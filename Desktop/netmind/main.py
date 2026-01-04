"""
Networking Agent API - main orchestrator

This module exposes the FastAPI application and wires the primary endpoints used by the
networking-agent backend. The high-level request flow is:

1. /parse-profile   -> accepts a LinkedIn URL, returns a normalized profile JSON using Proxycurl
2. /find-email      -> accepts name + company, returns a best-guess professional email (Hunter/Clearbit)
3. /generate-email  -> accepts profile + resume text, returns a generated subject/body via LLM
4. /demo/run        -> orchestrates the above steps end-to-end and optionally sends an email via Gmail
5. /gmail/auth      -> starts OAuth consent flow for Gmail
6. /gmail/callback  -> OAuth callback which exchanges code for tokens
7. /send-email      -> sends an email using stored Gmail OAuth tokens

Notes:
- Environment variables (see .env.example) are used for API keys and OAuth client IDs/secrets.
- OAuth tokens are persisted locally in gmail_tokens.json for now (do NOT commit this file).
- The demo orchestrator will return draft warnings when the LLM output contains phrases that may
  indicate potential hallucinations.

This file focuses on routing and orchestration; parsing, email finding, and LLM logic live in
`linkedin_parser.py`, `email_finder.py`, and `ai_writer.py` respectively.
"""

from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import os, requests
from dotenv import load_dotenv
from linkedin_parser import fetch_linkedin_profile
from email_finder import find_email
from ai_writer import generate_email
from gmail_auth import build_consent_url, exchange_code_for_tokens
from fastapi.responses import RedirectResponse
from gmail_sender import send_email as gmail_send


load_dotenv()

app = FastAPI(
    title="Networking Agent API",
    description="Backend for LinkedIn parsing, email discovery, and AI outreach",
    version="0.1.0"
)

# CORS (allow frontend later)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # tighten later in prod
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.get("/")
def home():
    return {"message": "NetMind backend running!"}


@app.get("/health")
def health_check():
    return {"status": "ok"}


def _safe_return_response(r: 'requests.Response'):
    """Return parsed JSON when possible, otherwise return status + text.

    This prevents the server from raising JSON decode errors when upstream
    returns non-JSON (HTML/error page) which caused 500s.
    """
    try:
        return r.json()
    except Exception:
        return {"status_code": r.status_code, "text": r.text}


@app.post("/chat")
def chat_llm(data: dict):
    headers = {
        "Authorization": f"Bearer {os.getenv('NIM_API_KEY')}",
        "Content-Type": "application/json",
    }
    payload = {
        "model": os.getenv("LLM_MODEL"),
        "messages": [
            {"role": "system", "content": "You are a helpful networking assistant."},
            {"role": "user", "content": data["text"]},
        ],
        "temperature": 0.6,
        "max_tokens": 512,
    }
    r = requests.post(f"{os.getenv('NIM_BASE_URL').rstrip('/')}/chat/completions", headers=headers, json=payload, timeout=30)
    if not r.ok:
        return _safe_return_response(r)
    return r.json()


# ---------- Embeddings ----------
class EmbedBody(BaseModel):
    text: str


@app.post("/embed")
def embed_text(body: EmbedBody):
    url = f"{os.getenv('NIM_BASE_URL').rstrip('/')}/embeddings"
    headers = {
        "Authorization": f"Bearer {os.getenv('NIM_API_KEY')}",
        "Content-Type": "application/json",
    }
    payload = {
        "model": os.getenv("EMBED_MODEL"),
        "input": [body.text],
        "input_type": "query",
    }
    r = requests.post(url, headers=headers, json=payload, timeout=30)
    if not r.ok:
        raise HTTPException(status_code=r.status_code, detail=r.text)
    return r.json()


# ---------- Guardrails (local) ----------
@app.post('/guardrails')
def guardrails_check(body: dict):
    messages = body.get('messages') or body.get('text') or []
    text = ''
    if isinstance(messages, list):
        text = ' '.join(m.get('content','') if isinstance(m, dict) else str(m) for m in messages)
    else:
        text = str(messages)

    violations = []
    if '@' in text and 'example.com' not in text:
        violations.append('possible email/address leakage')
    if 'ssn' in text.lower() or 'social security' in text.lower():
        violations.append('sensitive PII detected')
    if 'password' in text.lower():
        violations.append('credential mentioned')

    ok = len(violations) == 0
    suggestions = []
    if not ok:
        suggestions.append('Remove or redact PII before sending to the model')

    return {"ok": ok, "violations": violations, "suggestions": suggestions}


# ---------- Compose & Send Email (mock) ----------
@app.post('/compose_email')
def compose_email(data: dict):
    headers = {
        "Authorization": f"Bearer {os.getenv('NIM_API_KEY')}",
        "Content-Type": "application/json",
    }
    system = data.get('system') or "You are NetMind, an AI networking coach. Compose clear, professional emails."
    user_prompt = (
        f"Compose an email to: {data.get('to','<recipient>')}\nSubject: {data.get('subject','')}\nContext: {data.get('context','')}\nStyle: {data.get('style','professional')}"
    )
    payload = {
        "model": os.getenv('LLM_MODEL'),
        "messages": [
            {"role": "system", "content": system},
            {"role": "user", "content": user_prompt},
        ],
        "temperature": data.get('temperature', 0.3),
        "max_tokens": data.get('max_tokens', 400),
    }
    r = requests.post(f"{os.getenv('NIM_BASE_URL').rstrip('/')}/chat/completions", headers=headers, json=payload)
    try:
        resp = r.json()
    except Exception:
        resp = {"status_code": r.status_code, "text": r.text}

    email_text = None
    try:
        if isinstance(resp, dict) and 'choices' in resp:
            email_text = resp.get('choices', [])[0].get('message', {}).get('content')
    except Exception:
        email_text = None

    return {"email": email_text, "raw": resp}


class LinkedInRequest(BaseModel):
    url: str


@app.post("/parse-profile")
async def parse_profile(payload: LinkedInRequest):
    try:
        profile = await fetch_linkedin_profile(payload.url)
        return profile
    except Exception as e:
        return {"error": str(e)}


class FindEmailRequest(BaseModel):
    name: str
    company: str


@app.post("/find-email")
async def find_email_endpoint(payload: FindEmailRequest):
    try:
        result = await find_email(payload.name, payload.company)
        return result
    except Exception as e:
        return {"error": str(e)}


class GenerateEmailRequest(BaseModel):
    profile: dict
    resume_text: str = ""
    context: str = "networking"
    tone: str = "professional"


@app.post("/generate-email")
async def generate_email_endpoint(payload: GenerateEmailRequest):
    try:
        out = await generate_email(payload.profile, payload.resume_text, payload.context, payload.tone)
        return out
    except Exception as e:
        return {"error": str(e)}


class DemoRunRequest(BaseModel):
    linkedin_url: str
    resume_text: str = ""
    context: str = "networking"
    tone: str = "professional"
    send: bool = False


@app.post("/demo/run")
async def demo_run(payload: DemoRunRequest):
    """
    Demo run orchestrator — end-to-end pipeline.

    This endpoint runs the following stages sequentially:
      1. Parse the LinkedIn profile URL into a normalized `profile` JSON using Proxycurl.
      2. Derive a best-guess professional email for the person using name + company (Hunter/Clearbit).
      3. Generate a short, personalized email draft (subject + body) via the LLM using provided profile
         and resume snippets.
      4. Optionally send the generated draft via Gmail if `payload.send` is True.

    The function returns a JSON object containing:
      - profile: normalized LinkedIn profile
      - email: best-guess email address (or null)
      - confidence: numerical confidence for the email guess when available
      - sources: list of sources used for the email discovery
      - draft: the generated email draft (subject/body), may include `warnings` if validation flagged issues
      - sent: boolean indicating whether the system attempted to send the email
      - send_result: result from the Gmail sender when sending was requested (null otherwise)

    Important: this docstring is for documentation only and does not change behavior. Upstream errors are
    surfaced as error strings (improvements to error labeling will be handled in a later step).
    """
    try:
        # Stage 1: fetch profile from the LinkedIn URL
        profile = await fetch_linkedin_profile(payload.linkedin_url)
        name = profile.get("name") or profile.get("full_name") or ""
        company = profile.get("company") or ""

        # Stage 2: find candidate email
        email_result = await find_email(name, company)

        # Stage 3: generate draft via LLM
        draft = await generate_email(profile, payload.resume_text, payload.context, payload.tone)

        # Stage 4: optional send
        send_result = None
        if payload.send:
            # Ensure we have an email to send
            to_addr = email_result.get("email")
            if to_addr:
                send_result = gmail_send(
                    to=to_addr,
                    subject=draft.get("subject", "Quick question"),
                    body=draft.get("body", "")
                )
            else:
                send_result = {"error": "no recipient email available"}

        return {
            "profile": profile,
            "email": email_result.get("email"),
            "confidence": email_result.get("confidence"),
            "sources": email_result.get("sources", []),
            "draft": draft,
            "sent": bool(payload.send),
            "send_result": send_result,
        }
    except Exception as e:
        return {"error": str(e)}


@app.get('/gmail/auth')
def gmail_auth():
    try:
        url = build_consent_url()
        return RedirectResponse(url)
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


@app.get('/gmail/callback')
async def gmail_callback(code: str = None, state: str = None):
    if not code:
        raise HTTPException(status_code=400, detail='missing code')
    try:
        tokens = await exchange_code_for_tokens(code)
        return {"ok": True, "tokens": tokens}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


class SendEmailRequest(BaseModel):
    to: str
    subject: str
    body: str


@app.post("/send-email")
def send_email_route(payload: SendEmailRequest):
    try:
        result = gmail_send(
            to=payload.to,
            subject=payload.subject,
            body=payload.body
        )
        return result
    except Exception as e:
        return {"error": str(e)}



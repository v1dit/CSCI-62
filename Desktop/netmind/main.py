from fastapi import FastAPI, HTTPException
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import os, requests
from dotenv import load_dotenv
from linkedin_parser import fetch_linkedin_profile
from email_finder import find_email


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


@app.post('/send_email')
def send_email(email: dict):
    to = email.get('to')
    subject = email.get('subject')
    body_text = email.get('body') or email.get('email') or email.get('message')
    if not to or not subject or not body_text:
        return {"ok": False, "error": "missing to/subject/body"}
    try:
        with open('sent_emails.log', 'a') as f:
            f.write(f"TO: {to}\nSUBJECT: {subject}\nBODY:\n{body_text}\n---\n")
    except Exception:
        pass
    return {"ok": True, "message": "email queued (mock)", "to": to, "subject": subject}


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



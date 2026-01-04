import os
from dotenv import load_dotenv
import httpx

load_dotenv()
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")

OPENAI_CHAT_URL = "https://api.openai.com/v1/chat/completions"

PROMPT_SYSTEM = "You are NetMind, an AI assistant that writes concise, friendly, and professional outreach emails for networking. Keep emails short (3-6 sentences), personal, and action-oriented."

async def generate_email(profile: dict, resume_text: str = "", context: str = "networking", tone: str = "professional"):
    if not OPENAI_API_KEY:
        raise RuntimeError("OPENAI_API_KEY not set in environment")

    name = profile.get("name") or profile.get("full_name") or ""
    company = profile.get("company") or ""
    title = profile.get("title") or ""
    summary = profile.get("summary") or ""

    user_prompt = f"Generate a short subject and body for an outreach email.\nProfile name: {name}\nTitle: {title}\nCompany: {company}\nContext: {context}\nTone: {tone}\nProfile summary: {summary}\nResume snippets: {resume_text}\nRequirements: 1) subject line 3-8 words 2) body 3-6 sentences, friendly, include one specific detail from profile if available. Return output as JSON with keys 'subject' and 'body'."

    headers = {
        "Authorization": f"Bearer {OPENAI_API_KEY}",
        "Content-Type": "application/json",
    }
    payload = {
        "model": "gpt-4o-mini",
        "messages": [
            {"role": "system", "content": PROMPT_SYSTEM},
            {"role": "user", "content": user_prompt},
        ],
        "temperature": 0.65,
        "max_tokens": 400,
    }

    async with httpx.AsyncClient(timeout=30) as client:
        r = await client.post(OPENAI_CHAT_URL, headers=headers, json=payload)
        if r.status_code != 200:
            raise Exception(f"OpenAI error: {r.text}")
        data = r.json()

    # Extract model text and attempt to parse JSON inside
    try:
        text = data.get("choices", [])[0].get("message", {}).get("content", "")
    except Exception:
        text = ""

    # Try to find JSON in the response
    import re, json
    m = re.search(r"\{[\s\S]*\}", text)
    if m:
        try:
            out = json.loads(m.group(0))
            return out
        except Exception:
            pass

    # Fallback: return whole text in body
    return {"subject": f"Quick question about {company}", "body": text}

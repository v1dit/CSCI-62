import os
from dotenv import load_dotenv
import httpx

load_dotenv()
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")

OPENAI_CHAT_URL = "https://api.openai.com/v1/chat/completions"

PROMPT_SYSTEM = (
    "You are an assistant that writes short, professional networking emails.\n\n"
    "CRITICAL RULES:\n"
    "- Use ONLY the information explicitly provided in the profile data and resume text.\n"
    "- DO NOT invent projects, achievements, interests, or experiences.\n"
    "- If a detail is not provided, do NOT reference it.\n"
    "- DO NOT exaggerate the recipient’s role or impact.\n"
    "- DO NOT make assumptions about their work beyond their title and company.\n"
    "- Keep the email under 150 words.\n"
    "- The tone should be natural, human, and non-salesy.\n\n"
    "If there is insufficient information to personalize a sentence, omit personalization rather than guessing.\n\n"
    "Return the output strictly in valid JSON with the following structure: {\"subject\": \"...\", \"body\": \"...\"}\n"
)


async def generate_email(profile: dict, resume_text: str = "", context: str = "networking", tone: str = "professional"):
    if not OPENAI_API_KEY:
        raise RuntimeError("OPENAI_API_KEY not set in environment")

    name = profile.get("name") or profile.get("full_name") or ""
    company = profile.get("company") or ""
    title = profile.get("title") or ""
    summary = profile.get("summary") or ""

    user_prompt = (
        f"Generate a short subject and body for an outreach email.\n"
        f"Profile name: {name}\nTitle: {title}\nCompany: {company}\nContext: {context}\nTone: {tone}\nProfile summary: {summary}\nResume snippets: {resume_text}\n\n"
        "Requirements: 1) Return strictly valid JSON with keys 'subject' and 'body'. 2) subject 3-8 words. 3) body 3-6 sentences, friendly, include one specific detail from profile if available."
    )

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
        "temperature": 0.5,
        "max_tokens": 400,
    }

    async with httpx.AsyncClient(timeout=30) as client:
        r = await client.post(OPENAI_CHAT_URL, headers=headers, json=payload)
        if r.status_code != 200:
            raise Exception(f"OpenAI error: {r.text}")
        data = r.json()

    # Extract model text
    try:
        text = data.get("choices", [])[0].get("message", {}).get("content", "")
    except Exception:
        text = ""

    # Try to parse strict JSON
    import re, json
    m = re.search(r"\{[\s\S]*\}", text)
    parsed = None
    if m:
        try:
            parsed = json.loads(m.group(0))
        except Exception:
            parsed = None

    if not parsed:
        # If not parseable, fall back to a safe generic email
        parsed = {"subject": f"Quick question about {company}", "body": text}

    # Post-generation validation
    warnings = validate_email_against_profile(parsed.get("body", ""), profile)
    if warnings:
        parsed["warnings"] = warnings

    return parsed


def validate_email_against_profile(email_body: str, profile: dict) -> list:
    """
    Returns a list of suspicious phrases that may indicate hallucination.
    """
    red_flags = []
    forbidden_phrases = [
        "your work on",
        "your project",
        "your research",
        "i admired",
        "i was impressed by",
        "i loved",
        "congratulations on",
    ]

    body_lower = (email_body or "").lower()
    for phrase in forbidden_phrases:
        if phrase in body_lower:
            red_flags.append(phrase)

    # Education reference check
    if not profile.get("education") and "university" in body_lower:
        red_flags.append("education reference without education data")

    # Skills reference check
    if not profile.get("skills"):
        # if skills missing but email mentions common skill words, flag
        skill_markers = ["python", "react", "machine learning", "data science"]
        for s in skill_markers:
            if s in body_lower:
                red_flags.append(f"skill reference without skills data: {s}")

    return red_flags

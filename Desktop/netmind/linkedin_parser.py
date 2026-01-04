import os
import httpx
from dotenv import load_dotenv

load_dotenv()

PROXYCURL_API_KEY = os.getenv("PROXYCURL_API_KEY")

# Simple in-memory cache
PROFILE_CACHE = {}

PROXYCURL_ENDPOINT = "https://nubela.co/proxycurl/api/v2/linkedin"

async def fetch_linkedin_profile(linkedin_url: str):
    # Return cached result if available
    if linkedin_url in PROFILE_CACHE:
        return PROFILE_CACHE[linkedin_url]

    headers = {
        "Authorization": f"Bearer {PROXYCURL_API_KEY}"
    }

    params = {
        "url": linkedin_url,
        "fallback_to_cache": "on",
        "use_cache": "if-present"
    }

    async with httpx.AsyncClient(timeout=30) as client:
        response = await client.get(
            PROXYCURL_ENDPOINT,
            headers=headers,
            params=params
        )

    if response.status_code != 200:
        raise Exception(f"Proxycurl error: {response.text}")

    data = response.json()

    # Normalize fields
    profile = {
        "name": data.get("full_name"),
        "title": data.get("headline"),
        "company": data.get("experiences", [{}])[0].get("company", ""),
        "education": (
            data.get("education", [{}])[0].get("school", "")
            if data.get("education") else ""
        ),
        "skills": data.get("skills", []),
        "location": data.get("city"),
        "summary": data.get("summary")
    }

    PROFILE_CACHE[linkedin_url] = profile
    return profile

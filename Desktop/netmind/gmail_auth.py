import os
import json
from urllib.parse import urlencode
import httpx
from dotenv import load_dotenv

load_dotenv()

GOOGLE_CLIENT_ID = os.getenv('GMAIL_CLIENT_ID')
GOOGLE_CLIENT_SECRET = os.getenv('GMAIL_CLIENT_SECRET')
REDIRECT_URI = os.getenv('GMAIL_REDIRECT_URI', 'http://localhost:8000/gmail/callback')
SCOPES = [
    'https://www.googleapis.com/auth/gmail.send',
    'https://www.googleapis.com/auth/gmail.compose',
]
TOKENS_FILE = 'gmail_tokens.json'

OAUTH_URL = 'https://accounts.google.com/o/oauth2/v2/auth'
TOKEN_URL = 'https://oauth2.googleapis.com/token'


def build_consent_url(state: str = 'netmind'):
    if not GOOGLE_CLIENT_ID:
        raise RuntimeError('GMAIL_CLIENT_ID not set')
    params = {
        'response_type': 'code',
        'client_id': GOOGLE_CLIENT_ID,
        'redirect_uri': REDIRECT_URI,
        'scope': ' '.join(SCOPES),
        'access_type': 'offline',
        'include_granted_scopes': 'true',
        'state': state,
        'prompt': 'consent'
    }
    return f"{OAUTH_URL}?{urlencode(params)}"


async def exchange_code_for_tokens(code: str):
    if not GOOGLE_CLIENT_ID or not GOOGLE_CLIENT_SECRET:
        raise RuntimeError('Google client ID/secret not set')
    data = {
        'code': code,
        'client_id': GOOGLE_CLIENT_ID,
        'client_secret': GOOGLE_CLIENT_SECRET,
        'redirect_uri': REDIRECT_URI,
        'grant_type': 'authorization_code'
    }
    async with httpx.AsyncClient() as client:
        r = await client.post(TOKEN_URL, data=data, timeout=30)
    if r.status_code != 200:
        raise Exception(f"Token exchange failed: {r.text}")
    tokens = r.json()
    # persist tokens
    try:
        with open(TOKENS_FILE, 'w') as f:
            json.dump(tokens, f)
    except Exception:
        pass
    return tokens

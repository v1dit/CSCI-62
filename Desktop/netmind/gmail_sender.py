import base64
import json
import os
from email.message import EmailMessage
from google.oauth2.credentials import Credentials
from googleapiclient.discovery import build

TOKEN_FILE = "gmail_tokens.json"
SCOPES = ["https://www.googleapis.com/auth/gmail.send"]

def load_credentials():
    if not os.path.exists(TOKEN_FILE):
        raise RuntimeError("OAuth tokens not found. Authenticate first.")

    with open(TOKEN_FILE, "r") as f:
        token_data = json.load(f)

    return Credentials(
        token=token_data["access_token"],
        refresh_token=token_data.get("refresh_token"),
        token_uri="https://oauth2.googleapis.com/token",
        client_id=os.getenv("GMAIL_CLIENT_ID"),
        client_secret=os.getenv("GMAIL_CLIENT_SECRET"),
        scopes=SCOPES,
    )

def send_email(to: str, subject: str, body: str):
    creds = load_credentials()
    service = build("gmail", "v1", credentials=creds)

    message = EmailMessage()
    message.set_content(body)
    message["To"] = to
    message["From"] = "me"
    message["Subject"] = subject

    encoded_message = base64.urlsafe_b64encode(
        message.as_bytes()
    ).decode()

    send_body = {"raw": encoded_message}

    result = service.users().messages().send(
        userId="me",
        body=send_body
    ).execute()

    return {
        "status": "sent",
        "message_id": result["id"]
    }

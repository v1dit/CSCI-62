import os
import re
import httpx
from dotenv import load_dotenv

load_dotenv()

HUNTER_API_KEY = os.getenv("HUNTER_API_KEY")
CLEARBIT_API_KEY = os.getenv("CLEARBIT_API_KEY")

# Common candidate patterns
PATTERNS = [
    "{first}.{last}",
    "{first}{last}",
    "{f}{last}",
    "{first}.{l}",
    "{first}",
]

async def _find_company_domain(company: str):
    # If the input looks like a domain, return it
    if not company:
        return None
    if "." in company:
        return company

    # Try Clearbit company lookup if key is present
    fallback = company.lower().replace(" ", "") + ".com"
    if not CLEARBIT_API_KEY:
        return fallback

    url = "https://company.clearbit.com/v2/companies/find"
    params = {"name": company}
    headers = {"Authorization": f"Bearer {CLEARBIT_API_KEY}"}

    async with httpx.AsyncClient(timeout=10) as client:
        try:
            r = await client.get(url, params=params, headers=headers)
            if r.status_code == 200:
                d = r.json()
                dom = d.get("domain")
                if dom:
                    return dom
        except Exception:
            pass

    return fallback

async def _hunter_email_finder(first: str, last: str, domain: str):
    if not HUNTER_API_KEY or not domain:
        return None
    url = "https://api.hunter.io/v2/email-finder"
    params = {
        "first_name": first,
        "last_name": last,
        "domain": domain,
        "api_key": HUNTER_API_KEY,
    }
    async with httpx.AsyncClient(timeout=15) as client:
        r = await client.get(url, params=params)
        if r.status_code == 200:
            data = r.json().get("data") or {}
            email = data.get("email")
            score = data.get("score")  # Hunter returns a score sometimes
            return {"email": email, "score": score, "source": "hunter"}
    return None

async def _hunter_verify(email: str):
    if not HUNTER_API_KEY or not email:
        return None
    url = "https://api.hunter.io/v2/email-verifier"
    params = {"email": email, "api_key": HUNTER_API_KEY}
    async with httpx.AsyncClient(timeout=12) as client:
        r = await client.get(url, params=params)
        if r.status_code == 200:
            data = r.json().get("data") or {}
            # Hunter's verifier may return 'score' or 'confidence'
            score = data.get("score") or data.get("confidence")
            return {"email": email, "score": score, "source": "hunter_verifier"}
    return None

async def find_email(name: str, company: str):
    """Return best deduced email, confidence (0..1), and sources list."""
    name = (name or "").strip()
    company = (company or "").strip()

    if not name:
        raise ValueError("name is required")

    parts = re.split(r"\s+", name)
    first = parts[0]
    last = parts[-1] if len(parts) > 1 else ""
    f = first[0] if first else ""
    l = last[0] if last else ""

    domain = await _find_company_domain(company)

    # First try Hunter's email-finder
    results = []
    try:
        hunter_res = await _hunter_email_finder(first, last, domain)
        if hunter_res and hunter_res.get("email"):
            results.append({
                "email": hunter_res["email"],
                "score": hunter_res.get("score"),
                "source": hunter_res.get("source"),
            })
    except Exception:
        pass

    # Generate candidate emails using patterns
    candidates = []
    for p in PATTERNS:
        local = p.format(first=first.lower(), last=last.lower(), f=f.lower(), l=l.lower())
        candidates.append(f"{local}@{domain}")

    # Remove duplicates, ensure hunter suggestion first
    if results:
        # ensure candidate list contains hunter suggestion first
        hunter_email = results[0]["email"]
        if hunter_email and hunter_email not in candidates:
            candidates.insert(0, hunter_email)

    # Verify candidates with Hunter verifier in parallel
    async with httpx.AsyncClient(timeout=20) as client:
        tasks = []
        for email in candidates:
            tasks.append(_hunter_verify(email))
        verified = await httpx.AsyncClient().gather(*tasks) if False else None

    # NOTE: httpx doesn't have .gather — use asyncio
    import asyncio
    verified = await asyncio.gather(*[_hunter_verify(e) for e in candidates])

    scored = []
    for v in verified:
        if not v or not v.get("email"):
            continue
        score = v.get("score")
        try:
            # normalize score to 0..1
            if score is None:
                sc = 0.0
            else:
                sc = float(score)
                if sc > 1:
                    sc = sc / 100.0
                sc = max(0.0, min(1.0, sc))
        except Exception:
            sc = 0.0
        scored.append({"email": v.get("email"), "score": sc, "source": v.get("source")})

    # Pick the best
    if scored:
        best = max(scored, key=lambda x: x["score"])
        sources = list({s["source"] for s in scored if s.get("score", 0) > 0})
        return {"email": best["email"], "confidence": best["score"], "sources": sources}

    # Fallback: return first candidate with low confidence
    if candidates:
        return {"email": candidates[0], "confidence": 0.1, "sources": []}

    return {"email": None, "confidence": 0.0, "sources": []}

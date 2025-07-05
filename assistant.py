import google.generativeai as genai
import sys

GEMINI_API_KEY = "API-key"
genai.configure(api_key=GEMINI_API_KEY)
model = genai.GenerativeModel("gemini-1.5-pro-latest")

query = sys.argv[1]

try:
    response = model.generate_content(f"Only return Python code. No explanation:\n\n{query}")
    print(response.text.strip())
except Exception as e:
    print("Error:", e)

#!/usr/bin/env python3

html_content = """<html>
<head>
<title>Simple CGI Script</title>
</head>
<body>
<h1>Hello, this is a simple CGI script!</h1>
</body>
</html>
"""

content_length = len(html_content)

print("Content-Type: text/html")
print(f"Content-Length: {content_length}")
print("\r\n\r\n")
print(html_content)
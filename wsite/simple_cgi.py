#!/usr/bin/env python3

html_content = """
<html>
<head>
<title>Simple CGI Script</title>
</head>
<body>
<h1>Hello, this is a simple CGI script!</h1>
</body>
</html>
"""

print("Content-Type: text/html\r\n")
print("\r\n")
print(html_content)
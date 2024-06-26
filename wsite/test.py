#!/usr/bin/env python3

import cgi
import os

def parse_query_string(query_string):
    """Parse the query string into a dictionary."""
    parameters = {}
    if query_string:
        pairs = query_string.split('&')
        for pair in pairs:
            if '=' in pair:
                key, value = pair.split('=', 1)
                parameters[key] = value
    return parameters

def generate_html(parameters):
    """Generate HTML content based on the parsed parameters."""
    html_content = """
    <html>
    <head>
        <title>CGI Script Output</title>
    </head>
    <body>
        <h1>CGI Script Output</h1>
        <h2>Query String Parameters</h2>
        <ul>
    """
    for key, value in parameters.items():
        html_content += f"<li>{key}: {value}</li>"
    html_content += """
        </ul>
    </body>
    </html>
    """
    return html_content

def main():
    """Main function to handle CGI request and response."""
    # Get the query string from the environment variable
    query_string = os.environ.get('QUERY_STRING', '')

    # Parse the query string
    parameters = parse_query_string(query_string)

    # Generate HTML content
    html_content = generate_html(parameters)

    # Print the HTTP headers and HTML content
    print("Content-Type: text/html\n")
    print(html_content)

if __name__ == "__main__":
    main()
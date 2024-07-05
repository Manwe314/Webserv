#!/usr/bin/php
<?php
// Set the content type header to HTML
header('Content-Type: text/html');

$data = getenv('POST_DATA');

// Provide default data if no data is received
if (empty($data)) {
    $data = "No data received.";
}

// Specify the file path for the HTML file
$file_path = "/sgoinfre/bleclerc/github/Webserv/tests/cgi/newfile.html";

// Create the HTML content
$html_content = "
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>CGI Script Response</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f0f0f0;
            color: #333;
            padding: 20px;
        }
        .container {
            background-color: #fff;
            border: 1px solid #ccc;
            border-radius: 5px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            padding: 20px;
            max-width: 600px;
            margin: auto;
        }
        h1 {
            color: #007bff;
        }
        p {
            font-size: 1.1em;
        }
        .code-block {
            background-color: #f9f9f9;
            border-left: 5px solid #007bff;
            padding: 10px;
            font-family: monospace;
            white-space: pre-wrap;
            word-wrap: break-word;
        }
    </style>
</head>
<body>
    <div class='container'>
        <h1>File Creation Status</h1>
        <p><strong>File Name:</strong> newfile.html</p>
        <p><strong>Directory:</strong> /sgoinfre/bleclerc/github/Webserv/tests/cgi/</p>
        <p><strong>Content Inserted:</strong></p>
        <div class='code-block'>$data</div>
        <p>File created and data written successfully!</p>
    </div>
</body>
</html>
";

// Write the HTML content to the file
file_put_contents($file_path, $html_content);

// Set the content length header
$content_length = strlen($html_content);
header("Content-Length: $content_length");
header("Connection: close");

// Output the HTML content
echo $html_content;
?>

<?php
$html_content = "<html>
<head>
<title>Simple CGI Script</title>
</head>
<body>
<h1>Hello, this is a simple CGI script!</h1>
</body>
</html>
";

$content_length = strlen($html_content);

header("Content-Type: text/html");
header("Content-Length: " . $content_length);

echo $html_content;
?>
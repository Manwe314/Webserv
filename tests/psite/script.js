let sentData = {};

document.getElementById('updateButton').addEventListener('click', function() {
    // Get the values from the input fields
    const key1 = document.getElementById('key1').value;
    const key2 = document.getElementById('key2').value;

    // Define the URL and the data to be sent in the PUT request
    const url = 'http://localhost:8111/saves/jsonData.json';
    const data = {
        key1: key1,
        key2: key2
    };

    // Store the data in the global variable
    sentData = data;

    // Make the PUT request using Fetch API
    fetch(url, {
        method: 'PUT',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    })
    .then(response => {
        if (response.status === 204) {
            return null; // Handle 204 No Content explicitly
        } else if (response.ok) {
            return response.text(); // Read response as text to handle empty responses
        } else {
            throw new Error('Network response was not ok');
        }
    })
    .then(text => {
        if (text) {
            try {
                const data = JSON.parse(text); // Parse the response text as JSON
                console.log('Success:', data);
            } catch (error) {
                console.error('Error parsing JSON:', error);
            }
        }
    })
    .catch(error => {
        console.error('Error:', error);
        alert('There was an error making the PUT request.');
    });
});

document.getElementById('displayButton').addEventListener('click', function() {
    // Display the JSON data that was sent in the PUT request
    const jsonOutput = document.getElementById('jsonOutput');
    const jsonData = document.getElementById('jsonData');
    
    jsonData.textContent = JSON.stringify(sentData, null, 2);
    jsonOutput.style.display = 'block';
});

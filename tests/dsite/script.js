document.getElementById('deleteButton').addEventListener('click', function() {
    const uri = document.getElementById('uriInput').value.trim();

    if (uri === '') {
        alert('Please enter a URI');
        return;
    }

    fetch(uri, {
        method: 'DELETE'
    })
    .then(response => {
        if (response.ok) {
            return response.text();
        } else {
            throw new Error(response.statusText); // Throw error with status text
        }
    })
    .then(data => {
        document.getElementById('responseMessage').textContent = `DELETE request successful: ${data}`;
    })
    .catch(error => {
        console.error('Error:', error);
        document.getElementById('responseMessage').textContent = `Error deleting: ${error.message}`;
    });
});

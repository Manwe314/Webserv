const dropArea = document.getElementById('dropArea');
const fileInput = document.getElementById('fileInput');
const uploadButton = document.getElementById('uploadButton');
const status = document.getElementById('status');

let file;

dropArea.addEventListener('dragover', (event) => {
    event.preventDefault();
    dropArea.classList.add('dragging');
});

dropArea.addEventListener('dragleave', () => {
    dropArea.classList.remove('dragging');
});

dropArea.addEventListener('drop', (event) => {
    event.preventDefault();
    dropArea.classList.remove('dragging');
    file = event.dataTransfer.files[0];
    updateStatus();
});

fileInput.addEventListener('change', (event) => {
    file = event.target.files[0];
    updateStatus();
});

uploadButton.addEventListener('click', () => {
    if (file) {
        uploadFile(file);
    } else {
        status.textContent = 'No file selected.';
    }
});

function updateStatus() {
    status.textContent = `Selected file: ${file.name}`;
}

function uploadFile(file) {
    const url = 'http://localhost:8222/uploads/myfile';
    const formData = new FormData();
    formData.append('file', file);

    fetch(url, {
        method: 'PUT',
        body: formData
    })
    .then(response => {
        if (response.ok) {
            status.textContent = 'File uploaded successfully!';
        } else {
            throw new Error('File upload failed.');
        }
    })
    .catch(error => {
        console.error('Error:', error);
        status.textContent = 'There was an error uploading the file.';
    });
}

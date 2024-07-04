function onChangeImage() {
    const imageElement = document.getElementById('image');

    if (imageElement.src.includes('1.png')) {
        imageElement.src = 'assets/images/2.png';
    } else {
        imageElement.src = 'assets/images/1.png';
    }
}

function onSubmitForm(event) {
    event.preventDefault();

    const form = document.getElementById('contactForm');
    const formData = new FormData(form);

    fetch('/', {
        method: 'POST',
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById('formResponse').innerText = data;
    })
    .catch(error => {
        console.error('Error:', error);
    });

    return false; // Prevent default form submission
}

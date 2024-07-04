function onChangeImage() {
    const imageElement = document.getElementById('image');

    if (imageElement.src.includes('1.png')) {
        imageElement.src = 'assets/images/2.png';
    } else {
        imageElement.src = 'assets/images/1.png';
    }
}


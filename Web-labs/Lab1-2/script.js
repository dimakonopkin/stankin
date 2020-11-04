function Over1() {
    let text1 = document.getElementById("secret1");
        text1.innerHTML = "Конопкин";
        text1.style.color = "red";
        return;
}

function Out1() {
    let text1 = document.getElementById("secret1");
        text1.innerHTML = "Секретно";
        text1.style.color = "black";
        return;
}

function Over2() {
    let text1 = document.getElementById("secret2");
        text1.innerHTML = "Москва, Россия";
        text1.style.color = "red";
        return;
}

function Out2() {
    let text1 = document.getElementById("secret2");
        text1.innerHTML = "Секретно";
        text1.style.color = "black";
        return;
}

function Over3() {
    let text1 = document.getElementById("secret3");
    let jipg = document.getElementById("secret4");
        jipg.src = "./photo2.jpg";
        text1.innerHTML = "Данные удалены";
        text1.style.color = "red";
        return;
}

function Out3() {
    let text1 = document.getElementById("secret3");
    let jipg = document.getElementById("secret4");
        jipg.src = "./photo1.jpg";
        text1.innerHTML = "Секретно";
        text1.style.color = "black";
        return;
}

function ReturnArgument(arg) {
    return arg;
}

function ReturnSumArgument(arg1, arg2) {
    return arg1 + arg2;
}

//Функции для второй лабораторной.

function ReturnArg(a1) {
    return a1;
}

function SumArg(a1, a2) {
    return a1 + a2;
}

function QuadArr() {
    var stockArr = [5, 12, "r", "chack", 5, "soviet"];
    console.log("Изначальный массив элементов: ");
    console.log(stockArr);
    var newArr =[0,0,0,0,0,0];
    for (var i = 0; i < stockArr.length; i++)
    if (!isNaN(stockArr[i]))
        newArr[i] = stockArr[i] * stockArr[i];

    console.log("Полученный массив элементов: ");
    console.log(newArr);
}

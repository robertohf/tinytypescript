// let x, y, z : number;

// x = 10;
// z = 20;
// y = 30;

// console.log(x + y + z);

// if(x < y) {
//     console.log(x - z);
// }

// while(x < 20) {
//     if(x == 15) {
//         console.log("Hey it Works!");
//     }
//     x = x + 1;
// }

var addFunc = (num1: number, num2: number): number => {
    return num1 + num2;
};

function subFunc(num1: number, num2: number): number {
    return num1 - num2;
};

function multFunc(num1: number, num2: number): number {
    return num1 * num2;
};

function divFunc(num1: number, num2: number): number {
    return num1 / num2;
};

function modFunc(num1: number, num2: number): number {
    return num1 % num2;
};

function main () : void {
    console.log("Add: ");
    console.log(addFunc(5, 15));
    console.log("\n");

    console.log("Sub: ");
    console.log(subFunc(100, 75));
    console.log("\n");

    console.log("Mult: ");
    console.log(multFunc(5, 10));
    console.log("\n");

    console.log("Div: ");
    console.log(divFunc(50, 25));
    console.log("\n");

    console.log("Mod: ");
    console.log(modFunc(5, 50));
    console.log("\n");

    let x, y, z : number;

    let w : number;

    x = 20;
    y = 25;

    if(x < y) {
        z = x + y;
    }

    console.log(z);
};
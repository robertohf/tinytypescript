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

let addFunc = (num1: number, num2: number): number => {
    return num1 + num2;
};

let subFunc = (num1: number, num2: number): number => {
    return num1 - num2;
};

let multFunc = (num1: number, num2: number): number => {
    return num1 * num2;
};

let divFunc = (num1: number, num2: number): number => {
    return num1 / num2;
};

let modFunc = (num1: number, num2: number): number => {
    return num1 % num2;
};

let main = () : void => {
    let x, y, z : number;

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
};
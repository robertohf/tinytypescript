let arr = [10, 10, 10];
var num1:number = (10 + 20) * (10/2);
var num2 = 2;
num2 -= 2;

let res:number;
let flag:boolean = false;

if (num1 > num2) {
    res = num1 + num2;
    console.log(res);
} else {
    res = num1 - num2;
    console.log(res);
}

arr[1] = 15;

let myAdd = (x: number, y: number): number => {
    return x + y;
};

let mySyb = ():number => {
    return 10 + 5;
};

let num6:number = mySyb();
let num4:number = myAdd(10, 5);

for(let i = 0; i < 3; i++) {
    console.log ("for");
    continue;
}

while ( 5 < 5) {
    console.log ("while");
    break;
}

do {
    console.log ("do-while");
} while(1<num1);

for(let i in arr) {
    console.log ("for each");
}

num1++;
num2 -= 2;
num1--;
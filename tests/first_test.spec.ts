let x: number = 10;
let y = 20;
 
if(x%2 == 0){
  console.log("x is even");
}
else{
  console.log("x is odd");
}
if (x > y) 
{
    console.log('x is greater than y.');
} 
else if (x < y)
{
    console.log('x is less than y.'); //This will be executed
}
else if (x == y) 
{
    console.log('x is equal to y');
}

let a: number = 10;
let b = 20;
 
console.log("a = "+ a);
console.log("b = "+ b);
console.log("a+b = "+ a+b);
console.log("a-b = "+ a-b);
console.log("a*b = "+ a*b);	
console.log("a/b = "+ a/b);
console.log("a%b = "+ a%b);
console.log("a>b = "+ a>b);
console.log("a<b = "+ a<b);
console.log("a>=b = "+ a>=b);
console.log("a<=b = "+ a<=b);
console.log("true&&true = "+ true && false);

const bubbleSort = (arr: number[]): number[] => {
    const len = arr.length;
 
    for (let i = 0; i < len; i++) {
        for (let j = 0; j < len; j++) {
            if (arr[j] > arr[j + 1]) {
                [arr[j], arr[j + 1]] = [arr[j + 1], arr[j]];
            }
        }
    }
 
    return arr;
};

let sort = [5, 3, 1, 4, 6];

bubbleSort(sort);
let x, y, z : number;

x = 10;
z = 20;
y = 30;

console.log(x + y + z);

if(x < y) {
    console.log(x - z);
}

while(x < 20) {
    if(x == 15) {
        console.log("Hey it Works!");
    }
    x = x + 1;
} 

let bubbleSort = (num: number): number => {
    return num + num;
};
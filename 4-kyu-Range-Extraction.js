function solution(a){
    const x = a.sort((a, b) => a - b)
    let l, r, i, s = ""
    const p = () => s += "," + l + (l === r ? "" : ( r === 1 + l ? "," : "-") + r)
    for(l = r = x[0], i = 1; i < x.length; ++i){
        if (x[i] === 1 + r)  r = x[i]
        else p(), l = r = x[i]
    }
    p()
    return s.substring(1);
}

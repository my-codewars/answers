function triangle(s) {
    if (s.length <= 1)
        return s[s.length - 1];
    let n, res, range, i, j
    for (n = 1; 3 * n + 1 < s.length; ) n *= 3;
    for (res = [], range = [...Array(s.length - n).keys()].map(r => r), i = 0; i < range.length; ++i) {
        j = range[i];
        res.push(s[j] === s[j + n] ? s[j] : "R" !== s[j] && "R" !== s[j + n] ? "R" : "G" !== s[j] && "G" !== s[j + n] ? "G" : "B" !== s[j] && "B" !== s[j + n] ? "B" : void 0)
    }
    return triangle(res)
}

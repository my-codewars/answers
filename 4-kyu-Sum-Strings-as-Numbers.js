function sumStrings(a,b) {
    if (a.length < b.length) c = a, a = b, b = c ;
    a = a.split("")
    for(let i = a.length - 1, j = b.length - 1; j >= 0; --i, --j)
        a[i] = +a[i] + +b[j]
    for(let i = a.length - 1; i > 0; --i)
        if (a[i] > 9) a[i - 1] = +a[i - 1] + 1, a[i] -= 10
    return a.join("").replace(/^0+/, "")
}

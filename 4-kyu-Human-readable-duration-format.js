function formatDuration (s) {
    if (s < 1)
        return "now"
    const f = { 'year' : 31536000, 'day' : 86400, 'hour' : 3600, 'minute' : 60, 'second' : 1, }, r = [ ]
    for(const k in f){
        const v = ~~(s / f[k])
        if (v){
            s -= v * f[k]
            r.push("" + v + " " + k + (v > 1 ? "s" : ""))
        }
    }
    const and = r.pop()
    return r.length ? r.join(", ") + " and " + and : and ;
}

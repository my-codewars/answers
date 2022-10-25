"use strict";function regexDivisibleBy(o){var c,n,r,t,$=[],f=[],a=[],e=[];for(c=0;c<o;++c)for(n=0,$.push([]);n<o;++n)$[c].push(-1);for(c=0;c<o;++c)a.push(0),e.push(0);for(c=0;c<o;++c)for(n=0;n<2;++n)$[c][(2*c+n)%o]=n;for(c=0;c<o;++c)for(f.push([]),n=0;n<o;++n)f[c].push("");for(c=0;c<o;++c)for(n=0;n<o;++n)$[c][n]>=0&&(f[c][n]=c==n?"".concat($[c][n],"*"):"".concat($[c][n]),a[n]++,e[c]++);var u,h=function o(c){var n,r=0;for(n=0;n<c.length;++n)if("("==c[n]&&--r,")"==c[n]&&++r,"|"==c[n]&&!r)return 0;return 1},s=[],_=[],i=[];for(c=0;c<o;++c)s[c]=1;for(c=1;c<o;++c)_.push(c);for(c=1;c<o;++c)i[c]=Math.random();for(_.sort(function(o,c){return i[o]-i[c]}),u=0;u<o-1;++u)for(s[c=_[u]]=0,t=f[c][c].length?"(".concat(f[c][c],")*"):"",1==f[c][c].length?t="".concat(f[c][c],"*"):("1*"==f[c][c]||"0*"==f[c][c])&&(t=f[c][c]),n=0;n<o;++n)if(s[n]&&f[c][n].length){for(r=0;r<o;++r)if(s[r]&&f[r][c].length){var g=1==f[r][n].length?f[r][n]:"(".concat(f[r][n],")"),l=h(f[r][c])?f[r][c]:"(".concat(f[r][c],")"),p=h(f[c][n])?f[c][n]:"(".concat(f[c][n],")");f[r][n]=f[r][n].length?"".concat(g,"|(").concat(l).concat(t).concat(p,")"):"".concat(l).concat(t).concat(p)}}return 1==o?"1":"^(".concat(f[0][0],")+$")}

/*

function regexDivisibleBy(r) {
    var $, f, o, h, e = [],
        n = [],
        t = [],
        u = [];
    for ($ = 0; $ < r; ++$)
        for (f = 0, e.push([]); f < r; ++f) e[$].push(-1);
    for ($ = 0; $ < r; ++$) t.push(0), u.push(0);
    for ($ = 0; $ < r; ++$)
        for (f = 0; f < 2; ++f) e[$][(2 * $ + f) % r] = f;
    for ($ = 0; $ < r; ++$)
        for (n.push([]), f = 0; f < r; ++f) n[$].push("");
    for ($ = 0; $ < r; ++$)
        for (f = 0; f < r; ++f) e[$][f] >= 0 && (n[$][f] = $ == f ? `${e[$][f]}*` : `${e[$][f]}`, t[f]++, u[$]++);
    var _, s = r => {
            var $, f = 0;
            for ($ = 0; $ < r.length; ++$)
                if ("(" == r[$] && --f, ")" == r[$] && ++f, "|" == r[$] && !f) return 0;
            return 1
        },
        g = [],
        l = [],
        i = [];
    for ($ = 0; $ < r; ++$) g[$] = 1;
    for ($ = 1; $ < r; ++$) l.push($);
    for ($ = 1; $ < r; ++$) i[$] = Math.random();
    for (l.sort((r, $) => i[r] - i[$]), _ = 0; _ < r - 1; ++_)
        for (g[$ = l[_]] = 0, h = n[$][$].length ? `(${n[$][$]})*` : "", 1 == n[$][$].length ? h = `${n[$][$]}*` : ("1*" == n[$][$] || "0*" == n[$][$]) && (h = n[$][$]), f = 0; f < r; ++f)
            if (g[f] && n[$][f].length) {
                for (o = 0; o < r; ++o)
                    if (g[o] && n[o][$].length) {
                        var p = 1 == n[o][f].length ? n[o][f] : `(${n[o][f]})`,
                            a = s(n[o][$]) ? n[o][$] : `(${n[o][$]})`,
                            v = s(n[$][f]) ? n[$][f] : `(${n[$][f]})`;
                        n[o][f] = n[o][f].length ? `${p}|(${a}${h}${v})` : `${a}${h}${v}`
                    }
            }
    return 1 == r ? "1" : `^(${n[0][0]})+$`
}

*/

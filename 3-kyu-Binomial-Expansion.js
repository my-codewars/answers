function expand(expr) {
  const f=n=>!n||n*f(n-1)
  const g=(n,k)=>f(n)/(f(k)*f(n-k))
  const d=expr.match(/^\((.*)([a-z])(.*)\)\^(.*)$/i);
  [1,3,4].map(e=>d[e]=d[e]===""?1:d[e]=="-"?-1:+d[e])
  if (d[4] == 0) return "1"
  let r = [d[3]**d[4]]
  for(let i=0; i < d[4]; ++i)
    r[d[4]-i]=g(d[4],i)*d[1]**(d[4]-i)*d[3]**i
  r = r.map((v,k)=>v?(((v>=0?"+"+v:""+v)+(k>0?(d[2]+(k>1?"^"+k:"")):"")).replace(/([+-])1([a-z])/gi, "$1$2")):"").reverse().join("").replace(/^\+/,"")
  return r
}

#pragma once
// Dashboard page served by TelemetryDash (split in chunks: MSVC limits literal size).
static const std::string kDashboardHtml =
    std::string(R"HTML(<!doctype html><html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no">
<title>VirtualDashboard</title><link rel="icon" type="image/png" href="/favicon.png">
<style>
html,body{margin:0;background:#0b0e14;color:#e8edf5;font-family:system-ui,Segoe UI,sans-serif}
#wrap{max-width:1100px;margin:0 auto;padding:8px}
#gauges{display:flex;flex-wrap:wrap;justify-content:center;align-items:center;gap:4px}
canvas{width:min(46vw,380px);height:min(46vw,380px)}
#gearbox{width:min(30vw,160px);text-align:center}
#gear,#gearlbl{cursor:pointer}#gear{font-size:min(22vw,110px);font-weight:700;line-height:1}
#gearlbl{opacity:.6;letter-spacing:.2em;font-size:12px}#gear.sp{font-size:min(18vw,90px)}
#rev{display:none;width:min(98vw,150vh,1000px);height:auto}
#car{text-align:center;font-size:min(7vw,34px);font-weight:600;margin:6px 0 14px}
table{width:100%;border-collapse:collapse;font-size:15px}
td{padding:6px 8px;border-top:1px solid #1d2433}
td:last-child{text-align:right;font-variant-numeric:tabular-nums;color:#8fd3ff}
#foot{justify-content:space-between;align-items:center;margin:14px 8px 20px;color:#ff5fa8;font-size:14px}#foot a{color:#ff5fa8}#lt{text-align:center;color:#3a4252;margin-top:6px}#ind{display:flex;justify-content:center;gap:16px;color:#3a4252;margin-bottom:4px}#ind svg.on{color:#7fd13b}#rawbtn{text-align:center}#tog{background:none;border:0;color:#8a96a8;padding:4px 14px}#tog svg{transition:transform .2s}#tog.open svg{transform:rotate(180deg)}#lt.on{color:#7fd13b}#status{text-align:center;font-size:12px;opacity:.6;margin-top:10px}
@media (max-width:700px){#gauges{flex-direction:row}canvas{width:48vw;height:48vw}#gearbox{width:100%;order:3}}@media (orientation:landscape) and (max-height:600px){#gauges{flex-wrap:nowrap}canvas{width:min(34vw,66vh);height:min(34vw,66vh)}#gearbox{width:24vw;order:0}#gear,#gearlbl{cursor:pointer}#gear{font-size:min(14vw,34vh)}#gear.sp{font-size:min(11.5vw,28vh)}#car{font-size:min(4vw,6vh);margin:2px 0 6px}}
</style></head><body><div id="wrap">
<div id="gauges"><canvas id="spd" width="600" height="600"></canvas>
<div id="gearbox"><div id="ind"><svg id="indL" viewBox="0 0 100 100" width="34" height="34"><path d="M5 50L48 8V32H95V68H48V92Z" fill="currentColor"/></svg><svg id="indR" viewBox="0 0 100 100" width="34" height="34"><path d="M95 50L52 8V32H5V68H52V92Z" fill="currentColor"/></svg></div><div id="gear">-</div><div id="gearlbl">GEAR</div><div id="lt"><svg id="lights" viewBox="0 0 768 768" width="40" height="40"><g fill="none" stroke="currentColor" stroke-width="38" stroke-linecap="round" stroke-linejoin="round"><path d="M410 176C480 170 560 190 640 235C735 290 745 400 640 455C560 500 480 520 410 514C370 430 370 260 410 176Z"/><path d="M300 186L35 284M300 264L35 362M300 341L35 439M300 418L35 516M300 496L35 593"/></g></svg></div></div>
<canvas id="rpm" width="600" height="600"></canvas><canvas id="rev" width="1200" height="560"></canvas></div>
<div id="car">Waiting for the game...</div>

<div id="rawbtn"><button id="tog" aria-label="Show raw values"><svg viewBox="0 0 24 24" width="26" height="26"><path d="M5 9l7 7 7-7" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"/></svg></button></div>
<table id="raw" style="display:none"></table>
<div id="foot" style="display:none"><span>Created by suderek</span><span>v1.4.0</span><a href="https://fh2mods.com/" target="_blank" rel="noopener">Visit FH2mods</a></div>
<div id="status"></div></div>
<script>
)HTML") +
    std::string(R"HTML(
var d=null,mph=false,spdMax=300,ok=false,lastCar='',lastLim=0,testStart=0;
var testDown=false,turboSeen=false,nitroPrev=-1,pulseStart=0,layout=0,shownLayout=-1;
function nitroOn(){return ok&&d&&d.valid&&d.shownitro&&d.nitro>=0;}
function turboOn(){return ok&&d&&d.valid&&d.showturbo&&turboSeen;}
function nitroArc(c,W){
 var cx=W/2,cy=W/2,r=W*0.45,lw=W*0.035,hs=0.62,gap=0.05,n=4,seg=(2*hs-gap*(n-1))/n,pct=d.nitro;
 c.lineCap='butt';c.lineWidth=lw;
 for(var i=0;i<n;i++){
  var s=Math.PI-hs+i*(seg+gap),f=Math.max(0,Math.min(1,(pct-25*i)/25));
  c.strokeStyle='#3a1033';c.beginPath();c.arc(cx,cy,r,s,s+seg);c.stroke();
  if(f>0){c.strokeStyle='#c0129a';c.beginPath();c.arc(cx,cy,r,s,s+seg*f);c.stroke();}
 }
 if(pct<=0){
  var a=1;
  if(pulseStart){var el=(Date.now()-pulseStart)/1000;if(el<2)a=0.3+0.7*(0.5+0.5*Math.sin(el*Math.PI*4));}
  c.globalAlpha=a;c.strokeStyle='#ff2d3d';c.lineWidth=W*0.008;c.lineJoin='miter';
  for(var j=0;j<n;j++){
   var s2=Math.PI-hs+j*(seg+gap),e2=s2+seg;
   c.beginPath();c.arc(cx,cy,r+lw/2,s2,e2);c.arc(cx,cy,r-lw/2,e2,s2,true);c.closePath();c.stroke();
  }
  c.globalAlpha=1;
 }
}
function turboGauge(c,W){turboAt(c,W*0.885,W*0.115,W*0.075);}
function turboAt(c,cx,cy,r){
 var W=r/0.075,a0=Math.PI*0.75,sw=Math.PI*1.5;
 var bar=(d.boost-14.696)/14.5038,f=Math.max(0,Math.min(1,(bar+1)/4));
 c.lineCap='butt';c.lineWidth=W*0.016;
 c.strokeStyle='#1d2433';c.beginPath();c.arc(cx,cy,r,a0,a0+sw);c.stroke();
 c.strokeStyle='#ffb020';c.beginPath();c.arc(cx,cy,r,a0,a0+sw*f);c.stroke();
 c.fillStyle='#cfd8e6';c.font=(W*0.026)+'px sans-serif';c.textAlign='center';c.textBaseline='middle';
 for(var v=-1;v<=3;v++){var a=a0+sw*(v+1)/4;c.fillText(v,cx+Math.cos(a)*(r-W*0.04),cy+Math.sin(a)*(r-W*0.04));}
 var an=a0+sw*f;c.strokeStyle='#fff';c.lineWidth=W*0.007;c.beginPath();
 c.moveTo(cx,cy);c.lineTo(cx+Math.cos(an)*(r-W*0.06),cy+Math.sin(an)*(r-W*0.06));c.stroke();
 c.fillStyle='#fff';c.beginPath();c.arc(cx,cy,W*0.012,0,7);c.fill();
 c.font='bold '+(W*0.03)+'px sans-serif';c.fillText(bar.toFixed(1),cx,cy+r*0.8);
 c.font=(W*0.02)+'px sans-serif';c.fillStyle='#8a96a8';c.fillText('BAR',cx,cy+r*0.8+W*0.03);
}
function pulseA(){
 var a=1;
 if(pulseStart){var el=(Date.now()-pulseStart)/1000;if(el<2)a=0.3+0.7*(0.5+0.5*Math.sin(el*Math.PI*4));}
 return a;
}
function nitroSegs(c,x,y,w,h,vert,pct){
 var n=4,L=vert?h:w,gap=L*0.03,len=(L-gap*(n-1))/n;
 for(var i=0;i<n;i++){
  var pos=i*(len+gap),f=Math.max(0,Math.min(1,(pct-25*i)/25));
  var sx=vert?x:x+pos,sy=vert?y+h-pos-len:y,sw=vert?w:len,sh=vert?len:h;
  c.fillStyle='#3a1033';c.fillRect(sx,sy,sw,sh);
  if(f>0){c.fillStyle='#c0129a';if(vert)c.fillRect(sx,sy+sh*(1-f),sw,sh*f);else c.fillRect(sx,sy,sw*f,sh);}
  if(pct<=0){c.globalAlpha=pulseA();c.strokeStyle='#ff2d3d';c.lineWidth=Math.max(2,Math.min(sw,sh)*0.12);c.strokeRect(sx,sy,sw,sh);c.globalAlpha=1;}
 }
}
function stripCaption(c,t,x,y,u,al){c.fillStyle='#8a96a8';c.font=(u*0.028)+'px sans-serif';c.textAlign=al||'left';c.textBaseline='bottom';c.fillText(t,x,y);}
function altStrips(c,W,isSpd){
 var x=W*0.22,w=W*0.56,y=W*0.895,h=W*0.028;
 if(isSpd&&nitroOn()){stripCaption(c,'NITRO',x,y-W*0.012,W);nitroSegs(c,x,y,w,h,false,d.nitro);}
 if(!isSpd&&turboOn()){
  var bar=(d.boost-14.696)/14.5038,f=Math.max(0,Math.min(1,(bar+1)/4));
  stripCaption(c,'BOOST',x,y-W*0.012,W);stripCaption(c,bar.toFixed(1)+' bar',x+w,y-W*0.012,W,'right');
  c.fillStyle='#1d2433';c.fillRect(x,y,w,h);c.fillStyle='#ffb020';c.fillRect(x,y,w*f,h);
  c.fillStyle='#cfd8e6';c.font=(W*0.026)+'px sans-serif';c.textAlign='center';c.textBaseline='top';
  for(var v=-1;v<=3;v++)c.fillText(v,x+w*(v+1)/4,y+h+W*0.006);
 }
}
function blinkOn(){return Date.now()%600<300;}
function nitroRing(c,cx,cy,r,lw,a0,sw,pct){
 var n=4,gap=0.035,seg=(sw-gap*(n-1))/n;
 c.lineCap='butt';c.lineWidth=lw;
 for(var i=0;i<n;i++){
  var s=a0+i*(seg+gap),f=Math.max(0,Math.min(1,(pct-25*i)/25));
  c.strokeStyle='#0f2536';c.beginPath();c.arc(cx,cy,r,s,s+seg);c.stroke();
  if(f>0){c.save();c.shadowColor='#2aa8ff';c.shadowBlur=lw*0.9;c.strokeStyle='#38b6ff';c.beginPath();c.arc(cx,cy,r,s,s+seg*f);c.stroke();c.restore();}
 }
 if(pct<=0){
  c.globalAlpha=pulseA();c.strokeStyle='#ff2d3d';c.lineWidth=Math.max(2,lw*0.14);
  for(var j=0;j<n;j++){
   var s2=a0+j*(seg+gap),e2=s2+seg;
   c.beginPath();c.arc(cx,cy,r+lw/2,s2,e2);c.arc(cx,cy,r-lw/2,e2,s2,true);c.closePath();c.stroke();
  }
  c.globalAlpha=1;
 }
}
function turboRev(c,cx,cy,r){
 var FT='"Segoe UI","Helvetica Neue",Arial,sans-serif',a0=Math.PI*0.75,sw=Math.PI*1.5;
 var bar=(d.boost-14.696)/14.5038,f=Math.max(0,Math.min(1,(bar+1)/4));
 c.fillStyle='#070a10';c.beginPath();c.arc(cx,cy,r*1.2,0,7);c.fill();
 c.lineCap='butt';c.lineWidth=r*0.1;c.strokeStyle='#222a38';c.beginPath();c.arc(cx,cy,r,a0,a0+sw);c.stroke();
 c.save();c.shadowColor='#fff';c.shadowBlur=r*0.15;c.strokeStyle='#fff';c.beginPath();c.arc(cx,cy,r,a0,a0+sw*f);c.stroke();c.restore();
 for(var i=0;i<=8;i++){var a=a0+sw*i/8,mj=i%2===0;c.strokeStyle='#dfe6f0';c.lineWidth=mj?r*0.035:r*0.02;
  c.beginPath();c.moveTo(cx+Math.cos(a)*r*0.86,cy+Math.sin(a)*r*0.86);c.lineTo(cx+Math.cos(a)*r*(mj?0.7:0.76),cy+Math.sin(a)*r*(mj?0.7:0.76));c.stroke();}
 c.fillStyle='#e8edf5';c.font='italic 600 '+(r*0.2)+'px '+FT;c.textAlign='center';c.textBaseline='middle';
 for(var v=-1;v<=3;v++){var av=a0+sw*(v+1)/4;c.fillText(v,cx+Math.cos(av)*r*0.5,cy+Math.sin(av)*r*0.5);}
 var an=a0+sw*f;c.strokeStyle='#ff3b30';c.lineWidth=r*0.04;c.beginPath();c.moveTo(cx,cy);c.lineTo(cx+Math.cos(an)*r*0.62,cy+Math.sin(an)*r*0.62);c.stroke();
 c.fillStyle='#fff';c.font='italic 800 '+(r*0.3)+'px '+FT;c.fillText(bar.toFixed(1),cx,cy+r*0.98);
 c.fillStyle='#8a96a8';c.font='italic 600 '+(r*0.15)+'px '+FT;c.fillText('BAR',cx,cy+r*1.3);
}
function revuelto(v,sp){
 var cv=document.getElementById('rev'),c=cv.getContext('2d'),W=cv.width,H=cv.height;
 c.clearRect(0,0,W,H);
 var FT='"Segoe UI","Helvetica Neue",Arial,sans-serif';
 var cx=W/2,cy=H*0.55,R=H*0.40,a0=Math.PI*0.75,sw=Math.PI*1.5;
 var lim=v.limiter||8000,max=Math.ceil(lim/1000)*1000,red=(v.redline>0&&v.redline<lim)?v.redline:lim*0.93;
 var val=v.rpm,tl=testLevel();if(tl>=0)val=testDown?val+(max-val)*tl:max*tl;
 var f=Math.max(0,Math.min(1,val/max));
 if(nitroOn())nitroRing(c,cx,cy,R*1.15,R*0.07,a0,sw,d.nitro);
 else{c.lineWidth=R*0.012;c.strokeStyle='#16202c';c.beginPath();c.arc(cx,cy,R*1.15,a0,a0+sw);c.stroke();}
 c.fillStyle='#070a10';c.beginPath();c.arc(cx,cy,R*1.03,0,7);c.fill();
 c.lineCap='butt';
 c.lineWidth=R*0.03;c.strokeStyle='#222a38';c.beginPath();c.arc(cx,cy,R*0.985,a0,a0+sw);c.stroke();
 if(red<max){c.strokeStyle='#7d1a24';c.beginPath();c.arc(cx,cy,R*0.985,a0+sw*red/max,a0+sw);c.stroke();}
 var rf=Math.min(1,red/max),wf=Math.min(f,rf);
 c.save();c.shadowColor='#ffffff';c.shadowBlur=R*0.05;c.strokeStyle='#fff';
 c.beginPath();c.arc(cx,cy,R*0.985,a0,a0+sw*wf);c.stroke();
 if(f>rf){c.shadowColor='#ff3b30';c.strokeStyle='#ff3b30';c.beginPath();c.arc(cx,cy,R*0.985,a0+sw*rf,a0+sw*f);c.stroke();}
 c.restore();
 {var af=a0+sw*f,tip=f>rf?'#ff3b30':'#ff3b30';c.strokeStyle=tip;c.lineWidth=R*0.012;c.lineCap='butt';
  c.beginPath();c.moveTo(cx+Math.cos(af)*R*0.955,cy+Math.sin(af)*R*0.955);c.lineTo(cx+Math.cos(af)*R*1.015,cy+Math.sin(af)*R*1.015);c.stroke();}
 var steps=Math.round(max/200);
 for(var i=0;i<=steps;i++){
  var rv=i*200,a=a0+sw*rv/max,mj=i%5===0,r1=R*0.92,r2=mj?R*0.78:R*0.85;
  c.strokeStyle=rv>=red?'#e53a3a':'#dfe6f0';c.lineWidth=mj?R*0.028:R*0.012;
  c.beginPath();c.moveTo(cx+Math.cos(a)*r1,cy+Math.sin(a)*r1);c.lineTo(cx+Math.cos(a)*r2,cy+Math.sin(a)*r2);c.stroke();
 }
 c.fillStyle='#e8edf5';c.font='italic 600 '+(R*0.13)+'px '+FT;c.textAlign='center';c.textBaseline='middle';
 for(var k=0;k<=max/1000;k++){var ak=a0+sw*k*1000/max;c.fillStyle=k*1000>=red?'#ff5a5a':'#e8edf5';c.fillText(k,cx+Math.cos(ak)*R*0.66,cy+Math.sin(ak)*R*0.66);}
 c.fillStyle='#8a96a8';c.font='600 '+(R*0.07)+'px '+FT;c.fillText('GEAR',cx,cy-R*0.42);
 c.save();c.shadowColor='#ff3b30';c.shadowBlur=R*0.08;c.fillStyle='#ff3b30';
 c.font='italic 800 '+(R*0.5)+'px '+FT;c.fillText(gearTxt,cx,cy-R*0.1);c.restore();
 c.fillStyle='#fff';c.font='italic 800 '+(R*0.24)+'px '+FT;c.fillText(Math.round(sp),cx,cy+R*0.3);
 c.fillStyle='#8a96a8';c.font='italic 600 '+(R*0.08)+'px '+FT;c.fillText(mph?'MPH':'KM/H',cx,cy+R*0.46);
 c.fillStyle='#cfd8e6';c.font='italic 600 '+(R*0.09)+'px '+FT;c.fillText((rpmQ?Math.round(v.rpm/50)*50:Math.round(v.rpm))+' RPM',cx,cy+R*0.9);
 var ib=ok&&v.valid?(v.ind|0):0,on=blinkOn(),u=R*0.2;
 c.fillStyle=(ib&1)&&on?'#7fd13b':'#2a3140';c.save();c.translate(W*0.3,H*0.03);c.scale(u/100,u/100);c.fill(new Path2D('M5 50L48 8V32H95V68H48V92Z'));c.restore();
 c.fillStyle=(ib&2)&&on?'#7fd13b':'#2a3140';c.save();c.translate(W*0.7-u,H*0.03);c.scale(u/100,u/100);c.fill(new Path2D('M95 50L52 8V32H5V68H52V92Z'));c.restore();
 if(ok&&v.valid&&v.time>=0){var tm=Math.floor(v.time/60);
  c.fillStyle='#cfd8e6';c.font='italic 600 '+(R*0.12)+'px '+FT;c.textAlign='right';c.textBaseline='top';
  c.fillText(('0'+Math.floor(tm/60)).slice(-2)+':'+('0'+tm%60).slice(-2),W*0.97,H*0.04);}
 if(turboOn())turboRev(c,W*0.12,cy,H*0.17);
 shiftLight(c,W*0.12,H*0.15,H*0.045,v);
 assistCluster(c,W*0.88,cy,H*0.17,v);
}
function shiftLight(c,cx,cy,r,v){
 var live=ok&&v.valid&&v.redline>0,on=false;
 if(live){var d=v.rpm-v.redline;on=d>=-100;}
 c.fillStyle='#070a10';c.beginPath();c.arc(cx,cy,r*1.25,0,7);c.fill();
 c.save();if(on){c.shadowColor='#ff3b30';c.shadowBlur=r*1.2;}
 c.fillStyle=on?'#ff3b30':'#2a1216';c.beginPath();c.arc(cx,cy,r,0,7);c.fill();c.restore();
}
function assistIcon(c,cx,cy,s,txt,on){
 var FT='"Segoe UI","Helvetica Neue",Arial,sans-serif',col=on?'#e8edf5':'#3a4252';
 c.strokeStyle=col;c.fillStyle=col;c.lineWidth=s*0.12;c.beginPath();c.arc(cx,cy,s,0,7);c.stroke();
 c.font='italic 800 '+(s*(txt.length>2?0.58:0.8))+'px '+FT;c.textAlign='center';c.textBaseline='middle';c.fillText(txt,cx,cy+s*0.04);
}
function handbrakeIcon(c,cx,cy,s,on){
 c.save();c.translate(cx-s,cy-s);c.scale(s*2/768,s*2/768);
 c.strokeStyle=on?'#ff3b30':'#3a4252';c.lineWidth=48;c.lineCap='round';c.lineJoin='round';
 if(on){c.shadowColor='#ff3b30';c.shadowBlur=60;}
 c.beginPath();c.arc(384,384,255,0,7);c.stroke();
 c.stroke(new Path2D('M128 142C50 230 35 540 128 630M640 142C718 230 733 540 640 630M384 255V288M384 385V510'));
 c.restore();
}
function assistCluster(c,cx,cy,r,v){
 var live=ok&&v.valid,u=Math.max(2,r*0.2);
 c.fillStyle='#070a10';c.beginPath();c.arc(cx,cy,r*1.2,0,7);c.fill();
 c.lineWidth=r*0.1;c.strokeStyle='#222a38';c.beginPath();c.arc(cx,cy,r,0,7);c.stroke();
 assistIcon(c,cx-r*0.68,cy,u,'ABS',live&&v.abs>0);
 assistIcon(c,cx,cy-r*0.68,u,'TCS',live&&v.tcs>0);
 assistIcon(c,cx+r*0.68,cy,u,'STM',live&&v.stm>0);
 handbrakeIcon(c,cx,cy+r*0.68,u,live&&v.handbrake>0.5);
 c.save();c.translate(cx-r*0.25,cy-r*0.25);c.scale(r*0.5/768,r*0.5/768);
 c.strokeStyle=live&&v.lights?'#7fd13b':'#3a4252';c.lineWidth=38;c.lineCap='round';c.lineJoin='round';
 c.stroke(new Path2D('M410 176C480 170 560 190 640 235C735 290 745 400 640 455C560 500 480 520 410 514C370 430 370 260 410 176Z'));
 c.stroke(new Path2D('M300 186L35 284M300 264L35 362M300 341L35 439M300 418L35 516M300 496L35 593'));c.restore();
}
function testLevel(){
 if(!testStart)return -1;
 var e=(Date.now()-testStart)/1000,T=0.9;testDown=e>=T;
 if(e>=2*T){testStart=0;return -1;}
 var p=e<T?e/T:2-e/T;return p*p*(3-2*p);
}
document.getElementById('tog').onclick=function(){var t=document.getElementById('raw'),o=t.style.display==='none';t.style.display=o?'table':'none';document.getElementById('foot').style.display=o?'flex':'none';this.className=o?'open':'';};
document.getElementById('spd').onclick=function(){mph=!mph;};
document.getElementById('rpm').onclick=function(){rpmQ=!rpmQ;};
document.getElementById('rev').onclick=function(){mph=!mph;};
document.getElementById('gear').onclick=document.getElementById('gearlbl').onclick=function(){gearSpd=!gearSpd;};
function dial(cv,val,max,step,label,unit,red,fmt){
 var tl=testLevel();if(tl>=0)val=testDown?val+(max-val)*tl:max*tl;
 var c=cv.getContext('2d'),W=cv.width,cx=W/2,cy=W/2,r=W*0.42;
 c.clearRect(0,0,W,W);
 var sc=(layout===0&&((cv.id==='spd'&&nitroOn())||(cv.id==='rpm'&&turboOn())))?0.88:1,nc=layout===1?'#ff2d3d':'#fff';
 c.save();c.translate(cx,cy);c.scale(sc,sc);c.translate(-cx,-cy);
 var a0=Math.PI*0.75,sw=Math.PI*1.5;
 c.lineWidth=W*0.04;c.lineCap='butt';
 c.strokeStyle='#1d2433';c.beginPath();c.arc(cx,cy,r,a0,a0+sw);c.stroke();
 if(red>0&&red<max){c.strokeStyle='#7a1f2b';c.beginPath();c.arc(cx,cy,r,a0+sw*red/max,a0+sw);c.stroke();}
 var f=Math.max(0,Math.min(1,val/max));
 c.strokeStyle=(red>0&&val>=red)?'#ff4d5e':'#35b8ff';
 c.beginPath();c.arc(cx,cy,r,a0,a0+sw*f);c.stroke();
 c.fillStyle='#cfd8e6';c.font=(W*0.05)+'px sans-serif';c.textAlign='center';c.textBaseline='middle';
 for(var v=0;v<=max+0.001;v+=step){
  var a=a0+sw*v/max,x=cx+Math.cos(a)*(r-W*0.08),y=cy+Math.sin(a)*(r-W*0.08);
  c.fillText(fmt?fmt(v):v,x,y);
 }
 var a=a0+sw*f;c.strokeStyle=nc;c.lineWidth=W*0.012;c.beginPath();
 c.moveTo(cx,cy);c.lineTo(cx+Math.cos(a)*(r-W*0.14),cy+Math.sin(a)*(r-W*0.14));c.stroke();
 c.fillStyle=nc;c.beginPath();c.arc(cx,cy,W*0.03,0,7);c.fill();
 c.fillStyle='#fff';c.font='bold '+(W*0.13)+'px sans-serif';c.fillText((cv.id==='spd'&&gearSpd)?gearTxt:(cv.id==='rpm'&&rpmQ)?Math.round(val/50)*50:Math.round(val),cx,cy+W*0.2);
 c.font=(W*0.05)+'px sans-serif';c.fillStyle='#8a96a8';c.fillText(unit,cx,cy+W*0.3);
 c.restore();
 if(layout===0){
  if(cv.id==='spd'&&nitroOn())nitroArc(c,W);
  if(cv.id==='rpm'&&turboOn())turboGauge(c,W);
 }else if(layout===1)altStrips(c,W,cv.id==='spd');
}
function pretty(n){
 if(!n)return '';
 var m=n.match(/^(.*)_(\d\d)$/);
 var s=(m?m[1]:n).replace(/_/g,' ');
 return m?s+" '"+m[2]:s;
}
)HTML") +
    std::string(R"HTML(
var rpmQ=true,gearSpd=false,gearTxt='-';
function applyLayout(){
 if(shownLayout===layout)return;shownLayout=layout;
 var r=layout===2;
 document.getElementById('spd').style.display=r?'none':'';
 document.getElementById('rpm').style.display=r?'none':'';
 document.getElementById('gearbox').style.display=r?'none':'';
 document.getElementById('rev').style.display=r?'block':'none';
}
function draw(){
 applyLayout();
 var v=d&&d.valid?d:{speed:0,rpm:0,redline:6500,limiter:8000,gear:0};
 var k=mph?2.2369363:3.6,sp=v.speed*k;
 var g=v.gear;gearTxt=!ok||!v.valid?'-':(g<=0?'R':String(g));
 if(layout===2){revuelto(v,sp);return;}
 while(sp>spdMax&&testLevel()<0)spdMax+=50;
 dial(document.getElementById('spd'),sp,spdMax,spdMax>400?100:50,'',gearSpd?'GEAR':(mph?'MPH':'KM/H'),0);
 var lim=v.limiter||8000,rm=Math.ceil(lim/1000)*1000;
 dial(document.getElementById('rpm'),v.rpm,rm,1000,'','rpm',(v.redline>0&&v.redline<lim)?v.redline:lim*0.93,function(x){return x/1000;});
 document.querySelector('#spd').title=mph?'mph':'km/h';
 var ge=document.getElementById('gear');
 ge.textContent=gearSpd?(!ok||!v.valid?'-':Math.round(sp)):gearTxt;ge.style.fontSize='';ge.className=gearSpd?'sp':'';
 document.getElementById('gearlbl').textContent=gearSpd?(mph?'MPH':'KM/H'):'GEAR';
}
function rows(v){
 var t=document.getElementById('raw'),h='';
 function r(a,b){h+='<tr><td>'+a+'</td><td>'+b+'</td></tr>';}
 if(!v||!v.valid){t.innerHTML='';return;}
 r('Car (media name)',v.car);
 r('Speed (m/s)',v.speed.toFixed(2));
 r('Speed ('+(mph?'mph':'km/h')+' - tap speedometer to switch)',(v.speed*(mph?2.2369363:3.6)).toFixed(1));
 r('Engine speed (rpm)',v.rpm.toFixed(0));
 r('Red zone starts (rpm)',v.redline.toFixed(0));
 r('Rev limiter (rpm)',v.limiter.toFixed(0));
 r('Gear (raw)',v.gear);
 r('Throttle (0-1)',v.throttle.toFixed(3));
 r('Brake (0-1)',v.brake.toFixed(3));
 r('Clutch (0-1)',v.clutch.toFixed(3));
 r('Handbrake (0-1)',v.handbrake.toFixed(3));
 r('Engine torque (Nm)',v.torque.toFixed(1));
 r('Engine power (kW)',v.power.toFixed(1));
 if(v.nitro>=0)r('Nitro tank (%)',v.nitro);
 r('Turbo boost (bar)',((v.boost-14.696)/14.5038).toFixed(2));
 if(v.time>=0){var tm=Math.floor(v.time/60);r('Game time',('0'+Math.floor(tm/60)).slice(-2)+':'+('0'+tm%60).slice(-2));}
 r('Weather: atmosphere / rain / environment',v.w0.toFixed(3)+' / '+v.w1.toFixed(3)+' / '+v.w2.toFixed(3));
 r('Indicators (1=L 2=R 3=hazard)',v.ind);
 r('Steering (-1..1)',v.steer.toFixed(3));
 r('Headlights (0/1)',v.lights);
 r('ABS / TCS / STM on (0/1)',v.abs+' / '+v.tcs+' / '+v.stm);
 r('World position X',v.x.toFixed(2));
 r('World position Y (height)',v.y.toFixed(2));
 r('World position Z',v.z.toFixed(2));
 t.innerHTML=h;
}
function poll(){
 fetch('/api',{cache:'no-store'}).then(function(r){return r.json();}).then(function(j){
  d=j;ok=true;if(j.layout!==undefined)layout=j.layout|0;
  if(j.valid&&(j.car!==lastCar||j.limiter!==lastLim)){lastCar=j.car;lastLim=j.limiter;spdMax=300;testStart=Date.now();turboSeen=false;}
  if(j.valid&&Math.abs(j.boost)>0.01)turboSeen=true;
  if(j.valid&&j.nitro>=0){
   if(j.nitro<=0&&nitroPrev>0)pulseStart=Date.now();
   else if(j.nitro>0)pulseStart=0;
   nitroPrev=j.nitro;
  }else nitroPrev=-1;
  document.getElementById('car').textContent=j.valid?pretty(j.car):'Waiting for a car in the game...';
  document.getElementById('status').textContent='Connected';
  document.getElementById('lt').className=(j.valid&&j.lights)?'on':'';var ib=j.valid?(j.ind|0):0;document.getElementById('indL').setAttribute('class',((ib&1)&&(Date.now()%600<300))?'on':'');document.getElementById('indR').setAttribute('class',((ib&2)&&(Date.now()%600<300))?'on':'');rows(j);draw();setTimeout(poll,60);
 }).catch(function(){
  ok=false;d=null;document.getElementById('status').textContent='Connection lost, retrying...';
  document.getElementById('car').textContent='Waiting for a car in the game...';
  document.getElementById('lt').className='';document.getElementById('indL').setAttribute('class','');document.getElementById('indR').setAttribute('class','');rows(null);
  draw();setTimeout(poll,1000);
 });
}
draw();poll();setInterval(draw,33);
</script></body></html>
)HTML");

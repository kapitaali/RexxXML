% Technical diagrams for the RexxXML manual

%  The contents of this file are subject to the Mozilla Public License
%  Version 1.1 (the "License"); you may not use this file except in
%  compliance with the License. You may obtain a copy of the License at
%  http://www.mozilla.org/MPL/

%  Software distributed under the License is distributed on an "AS IS"
%  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
%  License for the specific language governing rights and limitations
%  under the License.

%  The Original Code is RexxXML.

%  The Initial Developer of the Original Code is Patrick TJ McPhee.
%  Portions created by Patrick McPhee are Copyright © 2003
%  Patrick TJ McPhee. All Rights Reserved.

%  Contributors:

% $Header: C:/ptjm/rexx/rexxxml/RCS/rexxxml.mp 1.5 2003/09/26 02:04:52 ptjm Rel $

% set prologues because we want to convert to pdf for use with pdftex
% set dvips = 1 on the command-line to generate prologue-less eps
if unknown dvips:
  prologues := 2;
fi;

% for consistency, make the illustrations all the same width, and one of a range
% of heights
width := 9cm;
short height := 3cm;
medium height := 6cm;
tall height := 12 cm;

% also for consistency, use a few thicknesses of pen
pickup penrazor xscaled 3pt rotated 30;
border nib = savepen; 

pickup penrazor xscaled .5pt rotated 30;
thin nib = savepen; 

pickup penrazor xscaled 1pt rotated 30;
normal nib = savepen; 

pickup penrazor xscaled 2pt rotated 30;
thick nib = savepen; 

picture short border, medium border, tall border;

pickup border nib;

bot lft z1 = top lft z2 - (0, short height)
            = top rt z3 - (width, short height)
            = bot rt z4 - (width, 0)
            = (0 , 0);

short border := image (draw z1--z2--z3--z4--cycle;);

numeric x[], y[];

bot lft z1 = top lft z2 - (0, medium height)
            = top rt z3 - (width, medium height)
            = bot rt z4 - (width, 0)
            = (0 , 0);

medium border := image (draw z1--z2--z3--z4--cycle;);

numeric x[], y[];

bot lft z1 = top lft z2 - (0, tall height)
            = top rt z3 - (width, tall height)
            = bot rt z4 - (width, 0)
            = (0 , 0);

tall border := image (draw z1--z2--z3--z4--cycle;);

% the center primary is spelled wrong and there are no primaries
% for the centres of the sides
vardef centre primary p = .5[llcorner p, urcorner p] enddef;
vardef lside primary p = .5[llcorner p, ulcorner p] enddef;
vardef rside primary p = .5[lrcorner p, urcorner p] enddef;
vardef tside primary p = .5[ulcorner p, urcorner p] enddef;
vardef bside primary p = .5[llcorner p, lrcorner p] enddef;

%%
%% the procedure instruction
%%

defaultfont := "ptmr8r";

beginfig(1);
  draw short border;
  numeric w[], t, b, d[];

  % positions of interior walls
  w1 = .25 width;
  w2 = .5 width;
  w3 = .75 width;

  % tops and bottoms of interior walls
  t = .9 short height;
  b = .1 short height;

  % doors in interior walls
  d1 = .7 short height;
  d3 = .3 short height;

  for i = 1, 3:
    pickup thick nib;
    draw (w[i], t)--(w[i], d[i]);
    draw (w[i], d[i]-8pt)--(w[i], b);
    pickup thin nib;
    draw (w[i]-4pt, d[i])--(w[i]+4pt, d[i]);
    draw (w[i]-4pt, d[i]-8pt)--(w[i]+4pt, d[i]-8pt);
  endfor;

  pickup thick nib;
  draw (w2, t)--(w2,b) withcolor .9 white;

  pair xv[], yv[], zv[];

  xv0 = (.25[0,w1], .3short height);
  xv1 = (.25[w1,w2], .3short height);
  xv2 = (.25[w2,w3], .5short height);
  xv3 = (.25[w3,width], .7short height);

  yv0 = (.5[0,w1], .7short height);
  yv1 = (.5[w1,w2], .3short height);
  yv2 = (.5[w2,w3], .5short height);
  yv3 = (.5[w3,width], .7short height);

  zv0 = (.75[0,w1], .5short height);
  zv1 = (.75[w1,w2], .3short height);
  zv2 = (.75[w2,w3], .5short height);
  zv3 = (.75[w3,width], .7short height);

  pickup normal nib;
  draw xv1 .. {left}(w1,d1-4pt) .. xv0 withcolor blue;
  draw xv2{.2,1} .. {left}(w1,d1-4pt) .. xv0 withcolor blue;
  draw xv3 .. {left}(w3,d3-4pt) .. {left}(w1,d1-4pt) .. xv0 withcolor blue;

  draw yv1{left} .. (w1+10pt,d1) .. {down}yv1 withcolor red;
  draw yv2 .. (w2,t) .. (w1+10pt,d1) .. {down}yv1 withcolor red;
  draw yv3 .. {left}(w3,d3-4pt) .. (.5[w1,w2],t) .. (w1+10pt,d1) .. {down}yv1 withcolor red;

  draw zv1 .. {left}(w1,d1-4pt) .. zv0 withcolor blue;
  draw zv2{.2,1} .. {left}(w1,d1-4pt) .. zv0 withcolor blue;
  draw zv3{down} .. (w3+10pt,d3) .. {up}zv3 withcolor green;

  for i = 0, 1, 2:
    label.bot ("x", xv[i]);
    label.bot ("y", yv[i]);
    label.bot ("z", zv[i]);
  endfor;

  label.top ("x", xv3);
  label.top ("y", yv3);
  label.top ("z", zv3);

  label.urt("expose x z", (w1, d1));
  label.urt("expose x y", (w3, d3));

  label.top("sub1", (.5[w1,w2], 0));
  label.top("sub2", (.5[w2,w3], 0));
  label.top("sub3", (.5[w3,width], 0));

endfig;

%
% tree representation of a ridiculous poem
%

def initpoem =
  draw medium border;
  string txt[];
  color clrs[];

  % need the document node at the top. This effectively includes the
  % XML declaration. Under that is a single comment, and a single poem
  % the poem has two stanzas, one of 4 lines and the other of 3 lines.

  % That's four levels of nodes, vertically, and seven horizontally at the bottom
  % I'll centre each stanza over its nodes, but spread everything else out evenly

  txt1 = "document"; txt2 = "comment"; txt3="elem:poem"; txt4=txt5="elem:stanza";
  for i=6 upto 12: txt[i] = endfor "elem:line";
  for i=13 upto 19: txt[i] = endfor "text";

  z1 = (.5width, 5/6medium height);
  z2 = (1/3width, 4/6medium height) = z3 - (1/3width, 0);
  z4 = (.5[x6,x9], 3/6medium height);
  z5 = (.5[x10,x12], 3/6medium height);

  for i = 1 upto 7: z[5+i] = ((i/8)*width, 2/6medium height);
                    z[12+i] = ((i/8)*width, 1/6medium height);
  endfor;

  % except that we get overlap that way
  for i = 2 step 2 until 7: y[5+i] := y[5+i] - 1/10medium height; endfor;
enddef;

% draw the element names
def drawpoemtext =
  picture p[];
  for i = 1 upto 19: p[i] := txt[i] infont defaultfont scaled defaultscale;
                     p[i] := p[i] shifted (z[i] - centre p[i]);
      draw p[i] withcolor clrs[i];
  endfor;
enddef;

def drawpoem =
  drawpoemtext;

  pickup normal nib;

  % draw lines according to the conceptual tree representation
  for i = 2, 3:
   draw bside p1{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  for i = 4,5:
   draw bside p3{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  for i = 6 upto 9:
   draw bside p4{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  for i = 10,11, 12:
   draw bside p5{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  for i = 6 upto 12:
     draw bside p[i]{down}..{down}tside p[i+7] withcolor clrs[i+7];
  endfor;
enddef;


beginfig(2);
  initpoem;

  for i = 1 upto 19:
     clrs[i] = black;
  endfor;

  drawpoem;
endfig;

%
% tree representation of a ridiculous poem with the context node and ancestors
% axis high-lighted
%

beginfig(3);
  initpoem;

  % context node
  context := 8;

  for i = 1 upto 19:
     clrs[i] = white;
%     clrs[i] = 46/255 red + 139/255 green + 87/255 blue;
  endfor;

  for i = 1,3,4:
     clrs[i] := black;
  endfor;

  clrs[context] := blue;

  drawpoem;
endfig;

%
% tree representation of a ridiculous poem with the context node and preceding
% axis high-lighted
%

beginfig(4);
  initpoem;

  % context node
  context := 8;

  for i = 1 upto 19:
     clrs[i] = white;
%     clrs[i] = 46/255 red + 139/255 green + 87/255 blue;
  endfor;

  for i = 1,2,3,4,6,7,13,14:
     clrs[i] := black;
  endfor;

  clrs[context] := blue;

  drawpoem;
endfig;

%
% RexxXML tree representation of a ridiculous poem
%

def rllabel(expr l, p, ps) =
  draw l rotated (angle(direction ps of p)-180)
       shifted (point ps of p);
enddef;

def lrlabel(expr l, p, ps) =
  draw l rotated (angle(direction ps of p))
       shifted (point ps of p);
enddef;



beginfig(5);
  initpoem;

  picture children, last, next;

  children = ("children" infont defaultfont scaled .5 defaultscale) shifted (0, 3pt);
  last = ("last" infont defaultfont scaled .5 defaultscale) shifted (0, 3pt);
  next = ("next" infont defaultfont scaled .5 defaultscale) shifted (0, 3pt);

  path pospath;

  for i = 1 upto 19:
     clrs[i] = black;
  endfor;

  drawpoemtext;

  % first- and last- links
  for i = 2, 3:
   draw bside p1{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  rllabel(children, bside p1{down}..{down}tside p2, .6);
  lrlabel(last, bside p1{down}..{down}tside p3, .5);

  % sibling link
  draw rside p2..lside p3;
  lrlabel(next, rside p2..lside p3, .5);

  % first- and last- links
  for i = 4,5:
   draw bside p3{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  rllabel(children, bside p3{down}..{down}tside p4, .5);
  lrlabel(last, bside p3{down}..{down}tside p5, .5);

  % sibling link
  draw rside p4..lside p5;
  lrlabel(next, rside p4..lside p5, .5);

  % first- and last- links
  for i = 6,9:
   draw bside p4{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  rllabel(children, bside p4{down}..{down}tside p6, .6);
  lrlabel(last, bside p4{down}..{down}tside p9, .5);

  % sibling links
  for i = 6 upto 8:
    draw rside p[i]{right}..{right}lside p[i+1];
    lrlabel(next, rside p[i]{right}..{right}lside p[i+1], .4);
  endfor;

  for i = 10, 12:
   draw bside p5{down}..{down}tside p[i] withcolor clrs[i];
  endfor;

  rllabel(children, bside p5{down}..{down}tside p10, .6);
  lrlabel(last, bside p5{down}..{down}tside p12, .5);

  % sibling links
  for i = 10,11:
    draw rside p[i]{right}..{right}lside p[i+1];
    lrlabel(next, rside p[i]{right}..{right}lside p[i+1], .4);
  endfor;

  for i = 6 upto 12:
     draw bside p[i]{down}..{down}tside p[i+7] withcolor clrs[i+7];
  endfor;

endfig;

%
% <eg>these words</eg> as they would be emitted by xslt processor
%
beginfig(6);
  draw short border;
  pickup normal nib;

  % put eg a little to the left of centre, these words a little to the right,
  % and space them evenly vertically

  z1 = (.5 width, 2/3 short height);
  z2 = (.5 width, 1/3 short height);

  picture p[];
  p1 = "elem:eg" infont defaultfont scaled defaultscale;
  p2 = "text: these words" infont defaultfont scaled defaultscale;

  p1 := p1 shifted (z1 - rside p1);
  p2 := p2 shifted (z2 - lside p2);

  draw p1;
  draw p2;
  draw rside p1{right}..{right}lside p2;

endfig;

end

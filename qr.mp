% Quick reference for RexxXML

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

% $Header: C:/ptjm/rexx/rexxxml/RCS/qr.mp 1.7 2003/10/29 04:28:02 ptjm Rel $

% set prologues because we want to convert to pdf for use with pdftex
prologues := 2;

defaultfont := "phvr8r";

% we need to do this twice -- once for A4 and once for US letter
width := 10 in;
height := 7.5 in;
pair offset;
offset = (.5in, .5in);

% for consistency, use a few thicknesses of pen
pickup penrazor xscaled 3pt rotated 30;
border nib = savepen; 

pickup penrazor xscaled .5pt rotated 30;
thin nib = savepen; 

pickup penrazor xscaled 1pt rotated 30;
normal nib = savepen; 

pickup penrazor xscaled 2pt rotated 30;
thick nib = savepen; 

color bg, bc[], bd[], tc[];

bg = (.4, .3, .8);

bc1 = red;
bd1 = red;
tc1 = black;
bc2 = blue;
bd2 = blue;
tc2 = black;
bc3 = (.5, .8., .2);
bd3 = (.5, .8., .2);
tc3 = black;
bc4 = (.2, .5, .4);
bd4 = (.2, .5, .4);
tc4 = black;
bc5 = (.3, 0, .3);
bd5 = (.3, 0, .3);
tc5 = white;
bc6 = (.6, .6, 0);
bd6 = (.6, .6, 0);
tc6 = black;
bc7 = (.8, .1, .5);
bd7 = (.8, .1, .5);
tc7 = black;

%special "[ /CropBox [ 0 0 " & decimal(width) & " " & decimal(height) & " ] /PAGES pdfmark";

def pseg(expr p, q) = (.1[p,q])---(.9[p,q]).. enddef;

def sec(expr tit, nstr)(text str, scr) = 
   begingroup;
    save voff;
    numeric voff;
    voff = 0;
    scr := image(
      for i = nstr downto 1:
        draw (str[i] infont defaultfont scaled defaultscale) shifted (0, voff);
        voff := voff + (defaultscale*11pt);
      endfor;
      draw (tit infont defaultfont scaled (1.2defaultscale)) shifted(0, voff+5pt);
    );
  endgroup;
enddef;

def scrbox =
  begingroup;
  save x, y, o;
  numeric x[], y[], o;
 
  o = 4pt;

  top rt z1 = llcorner scr - (o,o);
  bot rt z2 = ulcorner scr - (o,-o);
  bot lft z3 = urcorner scr + (o,o);
  top lft z4 = lrcorner scr +(o, -o);

  ((z1+(0,2o))---(z2-(0,2o))..(z2+(2o,0))---(z3-(2o,0))..(z3-(0,2o))---
   (z4+(0,2o))..(z4-(2o,0))---(z1+(2o,0))..cycle)
  endgroup
enddef;




def qr(expr page) =

beginfig(page);
  path p;
  pair pos;

  pickup border nib;

  bot lft z1 = (0,0);
  top lft z2 = (0,height);
  top rt z3 = (width,height);
  bot rt z4 = (width,0);

  p = pseg(z1,z2) pseg(z2,z3) pseg(z3,z4) pseg(z4,z1) cycle;

  fill (p shifted offset) withcolor bg;

  draw p shifted offset;

  picture title, scr;
  title = "RexxXML Quick Reference" infont defaultfont scaled 2defaultscale;
  % put the upper-left corner hear the upper-left border
  draw title shifted ((point 1.5 of p) - ulcorner title + (5pt,-5pt) + offset);

  string str[], tit;
  tit := "XML Processing";
  str[1] := "doc = xmlParseXML([url], [data], [flagSVD])";
  str[2] := "doc = xmlParseHTML([url], [data], [flagSVD], [encoding])";
  str[3] := "rc|str = xmlSaveDoc([url], doc, [stylesheet])";
  str[4] := "call xmlFreeDoc doc, [doc2, ...]";
  str[5] := "str = xmlNodeContent(node)";
  str[6] := "rc = xmlExpandNode(stemname, node)";

  sec(tit, 6)(str)(scr);
  pos := offset + (2in,3in);
  p := scrbox;
  fill (p shifted pos) withcolor bc1;
  draw (p shifted pos) withcolor bd1;
  draw (scr shifted pos) withcolor tc1;

  tit := "XSLT";
  str[1] := "ss = xmlParseXSLT([url], [expr])";
  str[2] := "ndoc = xmlApplyStylesheet(ss, doc, [flagUT], [flagUT], [parm, val, ...])";
  str[3] := "call xmlFreeStylesheet ss, [ss2, ...]";
  str[4] := "method = xmlOutputMethod(ss)";
  str[5] := "<rexx:rexx>script</rexx:rexx>";
  str[6] := "<rexx:function name='pref:name' [all-strings='true|false'] [return-type='number|tree|Boolean|node set|string']>script</rexx:function>";
  str[7] := "<rexx:template [all-strings='true|false'] [return-type='number|tree|Boolean|node set|string'] [var='value' ...]>script template</rexx:template>";
  str[8] := "xmlns:rexx='http://www.rexxla.org/xslt/rexxxml'";

  sec(tit, 8)(str)(scr);
  pos := offset + (.5in,5in);
  p := scrbox;
  fill (p shifted pos) withcolor bc2;
  draw (p shifted pos) withcolor bd2;
  draw (scr shifted pos) withcolor tc2;

  tit := "Housekeeping";
  str[1] := "call xmlLoadFuncs [noinit]";
  str[2] := "call xmlDropFuncs";
  str[3] := "verstr = xmlVersion()";
  str[4] := "errstr = xmlError()";
  str[5] := "call xmlFree thing, [thing2, ...]";

  sec(tit, 5)(str)(scr);
  pos := offset + (7in,6in);
  p := scrbox;
  fill (p shifted pos) withcolor bc3;
  draw (p shifted pos) withcolor bd3;
  draw (scr shifted pos) withcolor tc3;

  tit := "XPath";
  str[1] := "nodeset = xmlFindNode(expr, [node], [context])";
  str[2] := "string = xmlEvalExpression(expr, [node], [context])";
  str[3] := "number = xmlNodesetCount(nodeset)";
  str[4] := "node = xmlNodesetItem(nodeset, index)";
  str[5] := "cexpr = xmlCompileExpression(expr)";
  str[6] := "call xmlFreeExpression cexpr, [cexpr2, ...]";
  str[7] := "context = xmlNewContext(node, [nsdeclaration, ...])";
  str[8] := "rc = xmlSetContext(context, [node], [ndeclaration, ...])";
  str[9] := "call xmlFreeContext [context], [context2, ...]";
  str[10] := "call xmlNodesetAdd [nodeset], node, [node, ...]";

  sec(tit, 10)(str)(scr);
  pos := offset + (1in,1in);
  p := scrbox;
  fill (p shifted pos) withcolor bc4;
  draw (p shifted pos) withcolor bd4;
  draw (scr shifted pos) withcolor tc4;

  tit := "Building";
  str[1] := "doc = xmlNewDoc()";
  str[2] := "doc = xmlNewHTML()";
  str[3] := "node = xmlAddElement(node, name, [text], [where])";
  str[4] := "node = xmlAddAttribute(node, name, [text])";
  str[5] := "call xmlRemoveAttribute node, name, [name2, ...]";
  str[6] := "node = xmlAddText(node, text, [where])";
  str[7] := "node = xmlAddPI(node, name, [text], [where])";
  str[8] := "node = xmlAddComment(node, [text], [where])";
  str[9] := "node = xmlAddNode(node, child, [where])";
  str[10] := "node = xmlCopyNode(node)";
  str[11] := "call xmlRemoveContent node, [node, ...]";
  str[12] := "call xmlRemoveNode node, [node, ...]";

  sec(tit, 12)(str)(scr);
  pos := offset + (5in,1in);
  p := scrbox;
  fill (p shifted pos) withcolor bc5;
  draw (p shifted pos) withcolor bd5;
  draw (scr shifted pos) withcolor tc5;

  tit := "Schema";
  str[1] := "xsd = xmlParseSchema([url], [expr])";
  str[2] := "rc = xmlValidateDoc(xsd, doc, [flagUT], [flagUT])";
  str[3] := "call xmlFreeSchema xsd, [xsd2, ...]";
  str[4] := "call xmlDumpSchema file, xsd, [xsd2, ...]";

  sec(tit, 4)(str)(scr);
  pos := offset + (6in,4in);
  p := scrbox rotated -20;
  fill (p shifted pos) withcolor bc6;
  draw (p shifted pos) withcolor bd6;
  draw ((scr rotated -20) shifted pos) withcolor tc6;

  tit := "HTTP";
  str[1] := "data = xmlGet(url, [contvar])";
  str[2] := "data = xmlPost(url, [data], [format], [headers], [contvar])";

  sec(tit, 2)(str)(scr);
  pos := offset + (1in,4.3in);
  p := scrbox;
  fill (p shifted pos) withcolor bc7;
  draw (p shifted pos) withcolor bd7;
  draw (scr shifted pos) withcolor tc7;

currentpicture := (currentpicture rotated 90) shifted (height+2ypart offset, 0);

endfig;

enddef;


qr(1);


width := 277mm;
height := 190mm;

offset := (.5in, 10mm);

qr(2);

width := 10in;
height := 190mm;

offset := (10mm, 10mm);

qr(3);

bg := white;
for i = 1 upto 7:
  bc[i] := white;
  tc[i] := black;
endfor;

qr(4);


end

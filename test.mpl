program test;

var a, bb: char;
    cc: integer;

procedure ppp1;
begin
	return;
end;

var qqq123: array[32] of boolean;

procedure ppp2(a: integer; c: boolean);
var waa, err: boolean;
	ref: integer;
begin
	call ppp1;
end;

var a, bb: char;

procedure ppp2x(yy: char; xx: array[122] of boolean);
var wef: integer;
begin
{	writeln('a');
}	return;
end;

begin
	return;
	break;
	call ppp1;

	cc := 3;
	qqq123[12] := cc * 2 + 3;

	return;

	if (true) then begin
		break;
		return;
	end else begin
		break;
		return;
	end
	;

	while (true) do begin
		cc := 3;
		return;
	end
	;

	read(a, b, c);
	readln(asx);

	write(a, b, c);
	write('aaa');
	writeln(asx);
	writeln('ccc ccc');

	return;
end.

/* BUG: output string in procedure */

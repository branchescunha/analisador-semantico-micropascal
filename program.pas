program TesteCorreto;
var
  x, y : integer;
  z, w : real;
begin
  x := 10;
  y := x + 5;
  z := 3.14 * x;
  w := y / 2;
  
  if z > 15.0 then
  begin
    x := x + 1;
  end
  else
  begin
    x := x - 1;
  end;

  while x < 20 do
  begin
    x := x + 2;
  end
end.

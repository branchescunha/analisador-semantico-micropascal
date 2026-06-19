program TesteErros;
var
  x, y : integer;
  x : real; { Erro: Variavel x ja declarada }
  z : real;
begin
  x := 10;
  y := 3.14; { Erro: Atribuindo REAL a variavel INTEIRA }
  a := 5; { Erro: Variavel a nao declarada }
  z := x + a { Erro: a nao declarada }
end.

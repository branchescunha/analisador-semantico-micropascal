# Analisador Sintático e Semântico MicroPascal

Este projeto consiste em um analisador sintático e semântico para a linguagem **MicroPascal**, desenvolvido em **C** como trabalho acadêmico da disciplina de **Linguagens Formais, Autômatos e Compiladores**.

O analisador realiza a leitura de arquivos-fonte `.pas`, processa os tokens por meio de um scanner integrado, executa a análise sintática com um **parser preditivo descendente recursivo**, constrói uma **Árvore Sintática Abstrata (AST)** e valida regras semânticas relacionadas a declarações, tipos e uso de variáveis.

Além disso, a AST é exportada em formato **Graphviz**, permitindo a visualização gráfica da estrutura reconhecida pelo analisador.

---

## Objetivo

O objetivo do projeto é aplicar, na prática, conceitos fundamentais de compiladores, incluindo:

* análise léxica;
* análise sintática;
* parser descendente recursivo;
* regras de produção;
* construção de AST;
* análise semântica;
* tabela de símbolos;
* verificação de tipos;
* tratamento de erros;
* visualização gráfica com Graphviz.

---

## Fluxo do analisador

```text
Arquivo .pas
→ Scanner / Analisador Léxico
→ Tokens
→ Parser Preditivo Descendente Recursivo
→ AST
→ Análise Semântica
→ Exportação Graphviz
```

---

## Recursos implementados

* **Scanner integrado** para leitura e classificação dos tokens da linguagem.
* **Parser preditivo descendente recursivo** para validação sintática do programa.
* **Procedimento `CasaToken`** para consumir e validar tokens esperados.
* **Construção da AST** representando a estrutura abstrata do código analisado.
* **Exportação da AST em Graphviz**, gerando os arquivos `ast.dot` e `ast.png`.
* **Trace de regras de produção**, exibindo a sequência de derivações utilizadas.
* **Modo silencioso**, ocultando o trace de produção via parâmetro `-s`.
* **Suporte a sinais unários**, como em `x := -5;`.
* **Suporte a underscore em identificadores**, como `valor_total`.
* **Tratamento imediato de erros léxicos, sintáticos e semânticos**.
* **Análise semântica com tabela de símbolos e verificação de tipos**.

---

## Análise semântica

A análise semântica implementada realiza verificações como:

* registro de variáveis declaradas;
* suporte aos tipos `integer`, `real` e `boolean`;
* detecção de variáveis duplicadas;
* detecção de uso de variáveis não declaradas;
* verificação de compatibilidade de tipos em atribuições;
* validação de expressões booleanas em comandos `if` e `while`;
* restrição de operações inválidas envolvendo variáveis booleanas.

Exemplo de erro semântico detectado:

```pascal
x := (a < b) + 5;
```

Nesse caso, `(a < b)` resulta em `boolean`, portanto não pode ser usado em uma operação aritmética.

---

## Tratamento de erros

O analisador interrompe a execução no primeiro erro encontrado, retornando código de erro `1`.

Formatos de erro utilizados:

```text
nn:erro lexico [lex]
nn:token nao esperado [lex]
nn:fim de arquivo nao esperado
```

Também são tratados erros semânticos, como:

* variável não declarada;
* variável duplicada;
* incompatibilidade de tipos;
* condição não booleana;
* operação inválida com booleanos.

---

## Arquivos de teste

O repositório contém arquivos `.pas` usados para validar diferentes cenários de execução:

```text
program.pas
program_erro.pas
program_eof_erro.pas
program_if_erro.pas
program_lexico_erro.pas
program_real_erro.pas
program_sintatico_erro.pas
program_teste_novos.pas
program_type_error.pas
```

Esses arquivos incluem casos válidos e exemplos com erros léxicos, sintáticos e semânticos.

---

## Como compilar

No terminal, execute:

```bash
gcc -Wall -Wextra -o semantico semantico.c
```

---

## Como executar

Execução padrão, com geração da AST e exibição do trace de produções:

```bash
./semantico nome_do_arquivo.pas
```

Execução silenciosa, ocultando o trace de derivações:

```bash
./semantico -s nome_do_arquivo.pas
```

Exemplo:

```bash
./semantico program.pas
```

---

## Visualização da AST

Ao executar o programa com sucesso, são gerados os arquivos:

```text
ast.dot
ast.png
```

O arquivo `ast.dot` contém a descrição da árvore no formato Graphviz.

O arquivo `ast.png` contém a imagem da árvore gerada.

Caso a imagem não seja gerada automaticamente, execute:

```bash
dot -Tpng ast.dot -o ast.png
```

---

## Tecnologias utilizadas

* C
* GCC
* Graphviz
* MicroPascal
* Análise Léxica
* Análise Sintática
* Análise Semântica
* AST

---

## Status do projeto

Projeto acadêmico concluído.

O analisador implementa as principais etapas propostas para o trabalho: leitura de arquivos MicroPascal, análise léxica, análise sintática, geração da AST, análise semântica, validação de tipos e exportação da árvore com Graphviz.

---

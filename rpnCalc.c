
#include <stdio.h>
#include <stdlib.h>   /* for atof() */
#include <ctype.h>
#include <math.h>
#include <string.h>

#define MAXOP   100   /* max size of operand or operator */
#define NUMBER  '0'   /* signal that a number was found */
#define STACK   '1'
#define MATH    '2'
#define VARIABLE '3'
#define MAXVAL  100   /* maximum depth of val stack */

#define ENGLISH 26

int sp = 0;           /* next free stack position */
double val[MAXVAL];   /* value stack */
double variables[ENGLISH];

char buf[BUFSIZ];
size_t bufp = 0;

int getch(void) { return (bufp > 0) ? buf[--bufp] : getchar(); }

void ungetch_(int c) {
  if (bufp >= BUFSIZ) { fprintf(stderr, "ungetch: too many characters\n");  return; }
  buf[bufp++] = c;
}

/* push:  push f onto value stack */
void push(double f) {
  if (sp == MAXVAL) { fprintf(stderr, "stack overflow -- can't push %g\n", f);  return; }
  val[sp++] = f;
}

/* pop:  pop and return top value from stack */
double pop(void) {
  if (sp == 0) { fprintf(stderr, "stack underflow\n");  return 0.0; }
  return val[--sp];
}

int getop(char s[]) {
    int i, c;
    while ((s[0] = c = getch()) == ' ' || c == '\t') {;}
    s[1] = '\0';
    
    if (islower(c)) {
        i = 0;
      while (islower(s[++i] = c = getch())) {;}
      s[i] = '\0';
      if(c != EOF) { ungetch_(c); }
      return MATH;
    }
    
    if (!isdigit(c) && c != '.' && c != '-')
        return c;     /* not a number */
    
    i = 0;
    if (c == '-') {
        if(!isdigit(c = getch()) && c != '.'){
            if(c != EOF) { ungetch_(c); }
            return '-';
        }
        else
            s[++i]=c;
    }
    
    if (isdigit(c))   /* collect integer part */
        while (isdigit(s[++i] = c = getch())) {;}
    
    if (c == '.')     /* collect fraction part */
        while (isdigit(s[++i] = c = getch())) {;}

    
    s[i] = '\0';
    if (c != EOF)
        ungetch_(c);
    
    return NUMBER;
}

void modulo(char* s) {
    double op2 = pop();
    if(op2 != 0.0)
      push(fmod(pop(), op2));
    else
      fprintf(stderr, "zero divisor\n");
}

double topStack() {
    if (sp == 0) { fprintf(stderr, "stack is empty\n");  return 0.0; }
    printf ("top of stack is: \n\t");
    return val[sp - 1];
}

void swap(char s[]) {
    if (sp < 2) { fprintf(stderr, "not enough numbers to swap\n"); }
    double op2 = val[sp - 1];
    val[sp - 1] = val[sp - 2];
    val[sp - 2] = op2;
}

void clear_() {
    for(sp = 0; sp < MAXVAL; sp++) { val[sp] = 0.0; }
}

void math(char* s) {
  printf("s is: %s\n", s);
  double op1, op2, result = 0;

  if (strcmp(s, "pow") == 0) {
    op2 = pop();
    op1 = pop();
    printf("op = %f, op2 = %f\n", op1, op2);
    result = pow(op1, op2);
  }
  else if (strcmp(s, "sin") == 0) {
    result = sin(pop());
  }
  else if (strcmp(s, "cos") == 0) {
    result = cos(pop());
  }
  else if (strcmp(s, "exp") == 0) {
    result = exp(pop());
  }
  push(result);
  printf("%.2f\n", result);
}

void variable(int num) {
}

void rpn(void) {
    int type;
    double op2, v;
    char s[MAXOP];
    
    while ((type = getop(s)) != EOF) {
        switch (type) {
            case '\n': printf("\t%.8g\n", v = pop());  break;
        case NUMBER: push(atof(s));  break;
        case MATH: math(s);  break;
        case '?': topStack();  break;           //Displays the number on top of stack
        case '#': push(val[sp - 1]);  break;    //doubles the number
        case '$': swap(s);  break;
        case '~': clear_();  break;
        case '+': push(pop() + pop());  break;
        case '*': push(pop() * pop());  break;
        case '-':
                op2 = pop();
                push(pop() - op2);
                break;
        case '%': modulo(s);  break;
        case '/':
            if ((op2 = pop()) == 0.0) { fprintf(stderr, "divide by zero\n");  break; }
            push(pop() / op2);
            break;
        default:
                printf("error: unknown command %s\n", s);  break;
        }
    }
}

/* reverse Polish calculator */
int main(int argc, const char* argv[]) {
    rpn();
    return 0;
}

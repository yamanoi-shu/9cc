#include "9cc.h"

void gen_lvar(Node *node) {
  if (node->kind != ND_LVAR) {
    printf("代入の左辺値が変数ではありません");
    exit(1);
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  if (node->kind == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
    return;
  }
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lvar(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lvar(node->lhs);
      gen(node->rhs);
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
  } 

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  sete al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  case ND_NE:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  setne al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  case ND_LT:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  setl al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  case ND_LE:
    printf("  cmp rax, rdi\n"); // フラグレジスタに結果をセット
    printf("  setle al\n"); // 直前のcmpの結果を AL (RAXの下位8ビットにセット)
    printf("  movzb rax, al\n"); // 上位56ビットをゼロクリア
    break;
  }

  printf("  push rax\n");
}

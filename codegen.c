#include "9cc.h"

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
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

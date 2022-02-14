#include "9cc.h"

// ローカル変数
LVar *locals;

LVar *find_lvar(Token *token) {
  for (LKVar *var = locals; var; var = var->next) {
    if (var->len == token->len && !memcmp(token->loca, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *new_node_var(Token *token) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_LVAR;
  node->offset = (token->loc[0] - 'a' + 1) * 8;
  return node;
}

Node *parse(Token *token) {
  Node head;
  head.next = NULL;
  Node *cur = &head;
  while (token->kind != TK_EOF) {
    cur->next = stmt(&token, token);
    cur = cur->next;
  }

  return head.next;
}

Node *program(Token **rest, Token *token) {
  return stmt(rest, token);
}

Node *stmt(Token **rest, Token *token) {
  Node *node = expr(&token, token);
  if (!equal(token, ";")) {
    error_token(token, "expected '%s'", ";");
  }
  token = token->next;
  *rest = token;
  return node;
}

Node *expr(Token **rest, Token *token) {
  return assign(rest, token);
}

Node *assign(Token **rest, Token *token) {
  Node *node = equality(&token, token);
  if (equal(token, "=")) {
    node = new_node(ND_ASSIGN, node, assign(&token, token->next));
  }
  *rest = token;
  return node;
}

Node *equality(Token **rest, Token *token) {
  Node *node = relational(&token, token);
  for (;;) {
    if (equal(token, "==")) {
      node = new_node(ND_EQ, node, relational(&token, token->next));
      continue;
    }
    if (equal(token, "!=")) {
      node = new_node(ND_NE, node, relational(&token, token->next));
      continue;
    }

    *rest = token;
    return node;
  }
}

Node  *relational(Token **rest, Token *token) {
  Node *node = add(&token, token);
  for (;;) {
    if (equal(token, "<="))  {
      node = new_node(ND_LE, node, add(&token, token->next));
      continue;
    }
    if (equal(token, "<")) {
      node = new_node(ND_LT, node, add(&token, token->next));
      continue;
    }
    if (equal(token, ">=")) {
      node = new_node(ND_LE, add(&token, token->next), node);
      continue;
    }
    if (equal(token, ">")) {
      node = new_node(ND_LT, add(&token, token->next), node);
      continue;
    }
    *rest = token;
    return node;
  }
}

Node *add(Token **rest, Token *token) {
  Node *node = mul(&token, token);
  for (;;) {
    if (equal(token, "+")) {
      node = new_node(ND_ADD, node, mul(&token, token->next));
      continue;
    }
    if (equal(token, "-")) {
      node = new_node(ND_SUB, node, mul(&token, token->next));
      continue;
    }
    *rest = token;
    return node;
  }
}

Node *mul(Token **rest, Token *token) {
  Node *node = unary(&token, token);
  for (;;) {
    if (equal(token, "*")) {
      node = new_node(ND_MUL, node, unary(&token, token->next));
      continue;
    }
    if (equal(token, "/")) {
      node = new_node(ND_DIV, node, unary(&token, token->next));
      continue;
    }
    *rest = token;
    return node;
  }
}

Node *unary(Token **rest, Token *token) {
  if (equal(token, "+")) {
    return unary(rest, token->next);
  }
  if (equal(token, "-")) {
    return new_node(ND_SUB, new_node_num(0), unary(rest, token->next));
  }
  return primary(rest, token);
}

Node *primary(Token **rest, Token *token) {
  if (equal(token, "(")) {
    Node *node = expr(&token, token->next);
    if (!equal(token, ")")) {
      error_token(token, "expected '%s'", ")");
    }
    *rest = token->next;
    return node;
  }
  if (token->kind == TK_IDENT) {
    Node *node = new_node_var(token);
    *rest = token->next;
    return node;
  }
  if (token->kind == TK_NUM) {
    Node *node = new_node_num(token->val);
    *rest = token->next;
    return node;
  }

  error_token(token, "expected an expression");
}

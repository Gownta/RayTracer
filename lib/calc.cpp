#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include "calc.hpp"
using namespace std;

///////////////////////////////////////////////////////////////////////////////

struct StringPiece {
  StringPiece(const string & str) : str(str), start(0), end(str.length()) {}

  char peek() const {
    int s = start;
    while (s != end && str[s] == ' ') s++;
    if (s == end) return 0;
    return str[s];
  }

  char munch() {
    while (start != end && str[start] == ' ') start++;
    assert(start != end);
    char ret = str[start];
    start++;
    return ret;
  }

  double munch_double(int * rsz) {
    while (start != end && str[start] == ' ') start++;
    const char * dstart = str.data() + start;
    char * dend;
    double v = strtod(dstart, &dend);
    int sz = dend - dstart;
    assert(sz > 0);
    start += sz;
    assert(start <= end);
    if (rsz) *rsz = sz;
    return v;
  }

  string munch_id() {
    while (start != end && str[start] == ' ') start++;
    int sz = 0;
    while (start + sz < end && isalnum(str[start+sz])) sz++;
    assert(sz > 0);
    string id = str.substr(start, sz);
    start += sz;
    return id;
  }
  
  string str;
  int start, end;
};

///////////////////////////////////////////////////////////////////////////////

enum Kind {
  NUM,
  SYM,

  MINUS,

  ADD,
  SUB,
  MULT,
  DIV,
  POW
};

struct BTree {
  BTree * l;
  BTree * r;

  Kind kind;
  string lexeme;
  double val;

  void print(const string & lws = "") {
    cout << lws << lexeme << "\n";
    string nlws = "  " + lws;
    if (r) r->print(nlws);
    if (l) l->print(nlws);
  }
};

///////////////////////////////////////////////////////////////////////////////

static inline bool is_as(char c) { return c == '+' || c == '-'; }
static inline bool is_md(char c) { return c == '*' || c == '/'; }
static inline bool is_um(char c) { return c == '-'; }
static inline bool is_ex(char c) { return c == '^'; }

static BTree * parse_as(StringPiece & sp);

static BTree * parse_unit(StringPiece & sp) {
  if (sp.peek() == '(') {
    sp.munch();
    BTree * ret = parse_as(sp);
    assert(sp.peek() == ')');
    sp.munch();
    ret->lexeme = "(" + ret->lexeme + ')';
    return ret;
  }

  if (isalpha(sp.peek())) {
    string id = sp.munch_id();
    assert(id == "x" || id == "y" || id == "z");
    BTree * ret = new BTree();
    ret->l = ret->r = NULL;
    ret->kind = SYM;
    ret->lexeme = id;
    return ret;
  }

  {
    int sz;
    double v = sp.munch_double(&sz);
    BTree * ret = new BTree();
    ret->l = ret->r = NULL;
    ret->kind = NUM;
    ret->lexeme = sp.str.substr(sp.start - sz, sz);
    ret->val = v;
    return ret;
  }
}

static BTree * parse_ex(StringPiece & sp) {
  BTree * base = parse_unit(sp);

  if (sp.peek() == '^') {
    sp.munch();
    BTree * exp = parse_unit(sp);

    assert(exp->kind == NUM);
    assert(exp->val == 2 || exp->val == 3 || exp->val == 4);

    BTree * ret = new BTree();
    ret->l = base;
    ret->r = exp;
    ret->kind = POW;
    ret->lexeme = base->lexeme + '^' + exp->lexeme;

    return ret;
  }

  return base;
}

static BTree * parse_um(StringPiece & sp) {
  if (sp.peek() == '-') {
    sp.munch();
    BTree * inner = parse_ex(sp);
    
    BTree * ret = new BTree();
    ret->l = inner;
    ret->r = NULL;
    ret->kind = MINUS;
    ret->lexeme = "-" + inner->lexeme;
    
    return ret;
  }

  return parse_ex(sp);
}

static BTree * parse_md(StringPiece & sp) {
  BTree * ret = parse_um(sp);

  while (is_md(sp.peek())) {
    char as = sp.munch();
    BTree * next = parse_um(sp);

    BTree * nret = new BTree();
    nret->l = ret;
    nret->r = next;
    nret->kind = (as == '*' ? MULT : DIV);
    nret->lexeme = ret->lexeme + ' ' + as + ' ' + next->lexeme;

    if (as == '/') assert(next->kind == NUM);

    ret = nret;
  }

  return ret;
}

static BTree * parse_as(StringPiece & sp) {
  BTree * ret = parse_md(sp);

  while (is_as(sp.peek())) {
    char as = sp.munch();
    BTree * next = parse_md(sp);

    BTree * nret = new BTree();
    nret->l = ret;
    nret->r = next;
    nret->kind = (as == '+' ? ADD : SUB);
    nret->lexeme = ret->lexeme + ' ' + as + ' ' + next->lexeme;

    ret = nret;
  }

  return ret;
}

static BTree * parse(const string & str) {
  StringPiece sp(str);
  return parse_as(sp);
}

///////////////////////////////////////////////////////////////////////////////

static Factor operator*(const Factor & f1, const Factor & f2) {
  Factor ret;
  ret.k = f1.k * f2.k;
  #define GOWNTA_TMP(s) ret. s = f1. s + f2. s
    GOWNTA_TMP(pow_t);
    GOWNTA_TMP(pow_ox);
    GOWNTA_TMP(pow_oy);
    GOWNTA_TMP(pow_oz);
    GOWNTA_TMP(pow_rx);
    GOWNTA_TMP(pow_ry);
    GOWNTA_TMP(pow_rz);
    GOWNTA_TMP(pow_x);
    GOWNTA_TMP(pow_y);
    GOWNTA_TMP(pow_z);
  #undef  GOWNTA_TMP
  return ret;
}

static bool similar(const Factor & f1, const Factor & f2) {
  return
  #define GOWNTA_TMP(s) f1.s == f2.s &&
    GOWNTA_TMP(pow_t)
    GOWNTA_TMP(pow_ox)
    GOWNTA_TMP(pow_oy)
    GOWNTA_TMP(pow_oz)
    GOWNTA_TMP(pow_rx)
    GOWNTA_TMP(pow_ry)
    GOWNTA_TMP(pow_rz)
    GOWNTA_TMP(pow_x)
    GOWNTA_TMP(pow_y)
    GOWNTA_TMP(pow_z)
  #undef  GOWNTA_TMP
    true;
}

ostream & operator<<(ostream & os, const Factor & f) {
  os << f.k;
  #define GOWNTA_TMP(s) if (f.pow_ ## s) os << " * " << #s; if (f.pow_ ## s > 1) os << "^" << f.pow_ ## s;
    GOWNTA_TMP(t)
    GOWNTA_TMP(ox)
    GOWNTA_TMP(oy)
    GOWNTA_TMP(oz)
    GOWNTA_TMP(rx)
    GOWNTA_TMP(ry)
    GOWNTA_TMP(rz)
    GOWNTA_TMP(x)
    GOWNTA_TMP(y)
    GOWNTA_TMP(z)
  #undef  GOWNTA_TMP
  return os;
}

// turn an expression into a sum of factors
static vector<Factor> expand(BTree * tree, bool ext) {
  vector<Factor> ret;
  switch (tree->kind) {
    case NUM: {
      ret.emplace_back();
      ret.back().k = tree->val;
      break; }
    case SYM: {
      if (ext) {
        if (tree->lexeme == "x") {
          ret.emplace_back();
          ret.back().k = 1;
          ret.back().pow_ox = 1;
          ret.emplace_back();
          ret.back().k = 1;
          ret.back().pow_t = 1;
          ret.back().pow_rx = 1;
        } else if (tree->lexeme == "y") {
          ret.emplace_back();
          ret.back().k = 1;
          ret.back().pow_oy = 1;
          ret.emplace_back();
          ret.back().k = 1;
          ret.back().pow_t = 1;
          ret.back().pow_ry = 1;
        } else { // tree->lexeme == "z"
          ret.emplace_back();
          ret.back().k = 1;
          ret.back().pow_oz = 1;
          ret.emplace_back();
          ret.back().k = 1;
          ret.back().pow_t = 1;
          ret.back().pow_rz = 1;
        }
      } else {
        ret.emplace_back();
        ret.back().k = 1;
        if      (tree->lexeme == "x") ret.back().pow_x = 1;
        else if (tree->lexeme == "y") ret.back().pow_y = 1;
        else                          ret.back().pow_z = 1;
      }
      break; }
    case MINUS: {
      ret = expand(tree->l, ext);
      for (auto & f : ret) f.k = -f.k;
      break; }
    case ADD: {
      ret = expand(tree->l, ext);
      vector<Factor> other = expand(tree->r, ext);
      ret.insert(ret.end(), other.begin(), other.end());
      break; }
    case SUB: {
      ret = expand(tree->l, ext);
      vector<Factor> other = expand(tree->r, ext);
      for (auto & f : other) f.k = -f.k;
      ret.insert(ret.end(), other.begin(), other.end());
      break; }
    case MULT: {
      vector<Factor> l1 = expand(tree->l, ext);
      vector<Factor> l2 = expand(tree->r, ext);
      for (const auto & f1 : l1) for (const auto & f2 : l2) ret.push_back(f1*f2);
      break; }
    case DIV: {
      ret = expand(tree->l, ext);
      assert(tree->r->kind == NUM);
      for (auto & f : ret) f.k /= tree->r->val;
      break; }
    case POW: {
      vector<Factor> b = expand(tree->l, ext);
      ret = b;
      assert(tree->r->kind == NUM);
      int exp = tree->r->val;
      assert(exp >= 2 && exp <= 4);
      for (int i = 1; i < exp; ++i) {
        vector<Factor> acc;
        for (const auto & f1 : ret) for (const auto & f2 : b) acc.push_back(f1*f2);
        ret.swap(acc);
      }
      break; }
  }
  return ret;
}

static vector<Factor> collapse(const vector<Factor> & factors) {
  vector<Factor> ret;
  for (const auto & f : factors) {
    for (auto & r : ret) {
      if (similar(f, r)) {
        r.k += f.k;
        goto NEXT_FACTOR;
      }
    }
    ret.push_back(f);
    NEXT_FACTOR:;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////////

vector<Factor> parse_equation(const string & eqn, bool expand_t) {
  return collapse(expand(parse(eqn), expand_t));
}


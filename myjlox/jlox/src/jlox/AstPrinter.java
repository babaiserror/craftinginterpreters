package jlox;

class AstPrinter implements Expr.Visitor<String> {
    String print(Expr expr) {
        return expr.accept(this);
    }

    @Override
    public String visitAssignExpr(Expr.Assign expr) {
        return parenthesize("assign", expr.value);
    }

    @Override
    public String visitConditionalExpr(Expr.Conditional expr) {
        return parenthesize("conditional", expr.condition, expr.thenBranch, expr.elseBranch);
    }

    @Override
    public String visitBinaryExpr(Expr.Binary expr) {
        return parenthesize(expr.operator.lexeme, expr.left, expr.right);
    }

    public String visitFunctionExpr(Expr.Function expr) {
        String res = "function ( anonymous";
        for (int i=0; i < expr.parameters.size(); i++) {
            res += ", ";
            res += expr.parameters.get(i).lexeme;
        }
        res += " )";
        return res;
    }

    @Override
    public String visitCallExpr(Expr.Call expr) {
        Expr[] expressions = new Expr[expr.arguments.size()+1];
        expressions[0] = expr.callee;
        for (int i=0; i < expr.arguments.size(); i++) {
            expressions[i+1] = expr.arguments.get(i);
        }
        return parenthesize("function", expressions);
    }

    @Override
    public String visitGetExpr(Expr.Get expr) {
        return parenthesize("getter", expr);
    }

    @Override
    public String visitGroupingExpr(Expr.Grouping expr) {
        return parenthesize("group", expr.expression);
    }

    @Override
    public String visitLiteralExpr(Expr.Literal expr) {
        if (expr.value == null) return "nil";
        return expr.value.toString();
    }

    @Override
    public String visitLogicalExpr(Expr.Logical expr) {
        return parenthesize(expr.operator.lexeme, expr.left, expr.right);
    }

    @Override
    public String visitSetExpr(Expr.Set expr) {
        return parenthesize("set", expr);
    }

    @Override
    public String visitSuperExpr(Expr.Super expr) {
        return parenthesize("super", expr);
    }

    @Override
    public String visitThisExpr(Expr.This expr) {
        return parenthesize("this", expr);
    }

    @Override
    public String visitUnaryExpr(Expr.Unary expr) {
        return parenthesize(expr.operator.lexeme, expr.right);
    }

    public String visitVariableExpr(Expr.Variable expr) {
        return parenthesize("var", expr);
    }

    private String parenthesize(String name, Expr... exprs) {
        StringBuilder builder = new StringBuilder();

        builder.append("(").append(name);
        for (Expr expr : exprs) {
            builder.append(" ");
            builder.append(expr.accept(this));
        }
        builder.append(")");

        return builder.toString();
    }
}

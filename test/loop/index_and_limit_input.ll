; test case for simple loop, with its index and limit being function input values

define i32 @foo(i32 noundef %a, i32 noundef %limit) {
entry:
  %b = alloca i32, align 4
  store i32 7, ptr %b, align 4
  %cmp3 = icmp slt i32 %a, %limit
  br i1 %cmp3, label %for.body, label %for.end

for.body:                                         ; preds = %entry, %for.body
  %a.addr.04 = phi i32 [ %inc, %for.body ], [ %a, %entry ]
  %b.0.b.0.b.0.b.0. = load i32, ptr %b, align 4
  %add = add nsw i32 %b.0.b.0.b.0.b.0., 1
  store i32 %add, ptr %b, align 4
  %inc = add i32 %a.addr.04, 1
  %exitcond.not = icmp eq i32 %inc, %limit
  br i1 %exitcond.not, label %for.end, label %for.body

for.end:                                          ; preds = %for.body, %entry
  %b.0.b.0.b.0.b.0.1 = load i32, ptr %b, align 4
  ret i32 %b.0.b.0.b.0.b.0.1
}

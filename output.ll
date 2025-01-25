; ModuleID = 'YALLL'
source_filename = "YALLL"

define i32 @main() {
entry:
  br i1 true, label %if_true, label %if_false

if_true:                                          ; preds = %entry
  ret i32 1
  br label %if_exit

if_false:                                         ; preds = %entry
  br i1 true, label %else_if_true, label %else_if_false

else_if_true:                                     ; preds = %if_false
  ret i32 0
  br label %if_exit

else_if_false:                                    ; preds = %if_false
  br label %else_case

else_case:                                        ; preds = %else_if_false
  ret i32 1

if_exit:                                          ; preds = %else_if_true, %if_true
}

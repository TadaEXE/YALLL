func () : i32 {
bool true_val = true;
bool false_val = false;

bool or_val = true_val || false_val;
bool and_val = true_val && or_val && false_val;
return and_val;
}

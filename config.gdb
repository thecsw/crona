set breakpoint pending on
set confirm off
file ./cronac
break crona::Err::report
commands
	where
end
break crona::ToDoError::ToDoError
commands
	where
end
break crona::InternalError::InternalError
commands
	where
end

define t2
  set args p2_tests/$arg0.crona -p --
  run
end

define t3
  set args p3_tests/$arg0.crona -n --
  run
end

define t4
  set args p4_tests/$arg0.crona -c
  run
end

define t5
  set args p5_tests/$arg0.crona -a --
  run
end

define t6
  set args p6_tests/$arg0.crona -o --
  run
end

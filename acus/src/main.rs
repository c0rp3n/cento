use steel::steel_vm::engine::Engine;
use steel::steel_vm::register_fn::RegisterFn;

fn external_function(arg1: usize, arg2: usize) -> usize {
    arg1 + arg2
}

fn option_function(arg1: Option<String>) -> Option<String> {
    arg1
}

fn result_function(arg1: Option<String>) -> Result<String, String> {
    if let Some(inner) = arg1 {
        Ok(inner)
    } else {
        Err("Got a none".to_string())
    }
}

pub fn main() {
    let mut vm = Engine::new();

    // Here we can register functions
    // Any function can accept parameters that implement `FromSteelVal` and
    // return values that implement `IntoSteelVal`
    vm.register_fn("external-function", external_function);

    // See the docs for more information about `FromSteelVal` and `IntoSteelVal`
    // but we can see even functions that accept/return Option<T> or Result<T,E>
    // can be registered
    vm.register_fn("option-function", option_function);

    // Result values will map directly to errors in the VM and bubble back up
    vm.register_fn("result-function", result_function);

    steel_repl::run_repl(vm).unwrap();
}

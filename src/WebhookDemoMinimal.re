open ReasonReact;
open DivertiseasiaBsServiceWorker;

type state = {
  supported: bool,
  registration: option(ServiceWorker.Registration.t),
  unregisterSuccess: bool,
};

type action =
  | Supported
  | Registered(ServiceWorker.Registration.t)
  | UnregisterSuccess;

let successIndicator = (condition:bool, id:string) => {
  <span id=id key=id>
    {string(condition ? {j|✔|j} : {j|❌|j})}
  </span>
};

[@react.component]
let make = () => {
  let (state, dispatch) = React.useReducer(
    (state, action) =>
      switch (action) {
        | Supported => {...state, supported: true}
        | Registered(registration) => {...state, registration: Some(registration)}
        | UnregisterSuccess => {...state, unregisterSuccess: true}
      },
      {
        supported: false, 
        registration: None,
        unregisterSuccess: false,
      }
  );
  React.useEffect0(() => {
    switch(ServiceWorker.maybeServiceWorker) {
      | None => {
        Js.log("[App] Browser does *not* support service workers");
      }
      | Some(worker) => {
        Js.log("[App] Browser supports service workers");
        dispatch(Supported);
        open ServiceWorker;
        Window.addEventListener("load", () => {
          Js.Promise.(worker->register("demo-sw.js")
            |> then_((b:ServiceWorker.Registration.t) => {
              Js.log("[App] ServiceWorker registration successful with scope: " ++ b##scope);
              dispatch(Registered(b))
              resolve(Some(b));
            })
            |> catch(e => {
              Js.log2("[App] ServiceWorker registration failed: ", e);
              resolve(None)
            })
          ) |> ignore;
        });
      }
    };
    None;
  });
  let unregisterServiceWorker = (_) => {
    switch (state.registration) {
      | Some(registration) => {
        Js.Promise.(registration##unregister()
          |> then_((success:bool) => {
            if (success == true) {
              Js.log("[App] ServiceWorker unregister success");
              dispatch(UnregisterSuccess);
            }
            resolve(success);
          })
        ) |> ignore;
      }
      | None => {
        Js.log("[App] No ServiceWorker to unregister")
      }
    }
  };
  <div>
    <h1>{string("Minimal Demo")}</h1>
    <h2>{string("Your Browser:")}</h2>
    <table>
      <tbody>
        <tr key="supported"><td>{string("Supports ServiceWorker?")}</td><td>{successIndicator(state.supported, "supported")}</td></tr>
        <tr key="registration"><td>{string("Registered ServiceWorker?")}</td><td>{successIndicator(state.registration !== None, "registerSuccess")}</td></tr>
        <tr key="unregisterSuccess">
          <td>{string("Unregister?")}</td>
          <td>{switch(state.registration, state.unregisterSuccess) {
            | (Some(_), false) => <button id="unregister" onClick=unregisterServiceWorker>{string("Unregister")}</button>
            | _ => successIndicator(state.unregisterSuccess, "unregisterSuccess")
          }}
          </td>
        </tr>
      </tbody>
    </table>
    <h2>{string("Other Demos")}</h2>
    <ul>
      <li><a href="index.html">{string("Current:Minimal Demo")}</a></li>
      <li><a href="demo-full.html">{string("Full Demo (all of the features)")}</a></li>
      <li><a href="indexJS.html">{string("Full Demo JS (JS equilvalent of Full Demo)")}</a></li>
      <li><a href="features.html">{string("See Features (this page can be cached)")}</a></li>
    </ul>
  </div>
}
open ReasonReact;
open DivertiseasiaBsServiceWorker;

type state = {
  supported: bool,
  registerSuccess: bool,
  registration: option(ServiceWorker.ServiceWorkerRegistration.t),
  failureSuccess: bool,
  unregisterSuccess: bool,
};

type action =
  | Supported(bool)
  | Registered(ServiceWorker.ServiceWorkerRegistration.t)
  | UnregisterSuccess
  | SuccessfulFailure;

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
        | Supported(supported) => {...state, supported}
        | Registered(registration) => {...state, registerSuccess: true, registration: Some(registration)}
        | UnregisterSuccess => {...state, unregisterSuccess: true}
        | SuccessfulFailure => {...state, failureSuccess:true}
      },
      {
        supported: false, 
        registerSuccess: false,
        registration: None,
        unregisterSuccess: false,
        failureSuccess: false,
      }
  );
  React.useEffect0(() => {
    if (ServiceWorker.isSupported()) {
      Js.log("[App] Browser supports service workers");
      dispatch(Supported(true));
      ServiceWorker.windowAddEventListener("load", () => {
        Js.Promise.(ServiceWorker.register("demo-sw.js")
          |> then_((b:ServiceWorker.ServiceWorkerRegistration.t) => {
            Js.log("[App] ServiceWorker registration successful with scope: " ++ b#scope);
            dispatch(Registered(b))
            resolve(Some(b));
          })
          |> catch(e => {
            Js.log2("[App] ServiceWorker registration failed: ", e);
            resolve(None)
          })
        ) |> ignore;
        Js.Promise.(ServiceWorker.registerJs("nonexistant-sw.js")
          |> then_((b:ServiceWorker.ServiceWorkerRegistration.js) => {
            resolve(Some(b));
          })
          |> catch(e => {
            Js.log2("[App] ServiceWorker registration failed (expected): ", e);
            dispatch(SuccessfulFailure);
            resolve(None)
          })
        );
      })
    } else {
      dispatch(Supported(false));
      Js.log("[App] Browser does *not* support service workers");
    }
    None;
  });
  let unregisterServiceWorker = (_) => {
    switch (state.registration) {
      | Some(registration) => {
        Js.Promise.(registration#unregister()
          |> then_((success:bool) => {
            if (success == true) {
              Js.log("[App] ServiceWorker unregister success");
              dispatch(UnregisterSuccess);
            } else {
              Js.log("[App] ServiceWorker unregister failed");
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
    <h1>{string("Full Demo")}</h1>
    <h2>{string("Your Browser:")}</h2>
    <table>
      <tbody>
        <tr key="supported"><td>{string("Supports ServiceWorker?")}</td><td>{successIndicator(state.supported, "supported")}</td></tr>
        <tr key="registration"><td>{string("Registered ServiceWorker now?")}</td><td>{successIndicator(state.registerSuccess, "registerSuccess")}</td></tr>
        <tr key="failureSuccess"><td>{string("Registering Nonexistant ServiceWorker fails?")}</td><td>{successIndicator(state.failureSuccess, "failureSuccess")}</td></tr>
        <tr key="unregisterSuccess">
          <td>{string("Unregister works?")}</td>
          <td>{(state.registration !== None && state.unregisterSuccess === false) ? <button id="unregister" onClick=unregisterServiceWorker>{string("Unregister")}</button> : successIndicator(state.unregisterSuccess, "unregisterSuccess")}</td>
        </tr>
      </tbody>
    </table>
    <h2>{string("Service Worker from Registration")}</h2>
    {
      switch(state.registration) {
        | None => ReasonReact.null
        | Some(registration) => {
          switch (registration#worker) {
            | None => ReasonReact.null
            | Some(x) => {
              <table>
                <tbody>
                  <tr><td>{string("Script URL")}</td><td>{string(x.scriptURL)}</td></tr>
                  <tr><td>{string("Current State")}</td><td>{string(x.state)}</td></tr>
                  // <tr><td>{string("State Found In Registration")}</td><td>
                  //   {string({switch (registration.active, registration.waiting, registration.installing) {
                  //     | (Some(_), _, _) => "active"
                  //     | (None, Some(_), _) => "waiting"
                  //     | (None, None, Some(_)) => "installing"
                  //     | _ => "n/a"
                  //   }})}
                  // </td></tr>
                </tbody>
              </table>
            }
          }
        }
      }
    }
    <h2>{string("Service Worker from Controller")}</h2>
    {
      switch(ServiceWorker.getController()) {
        | None => ReasonReact.null
        | Some(x) => {
          <table>
            <tbody>
              <tr><td>{string("Script URL")}</td><td>{string(x.scriptURL)}</td></tr>
              <tr><td>{string("State")}</td><td>{string(x.state)}</td></tr>
            </tbody>
          </table>
        }
      }
    }
    <h2>{string("Offline Feature")}</h2>
    <p>{string("Make sure you have the service worker still registered!" )} <a href="features.html#offline">{string("Click this link! ")}</a></p>
    <h2>{string("Other Demos")}</h2>
    <ul>
      <li><a href="index.html">{string("Minimal Demo")}</a></li>
      <li><a href="demo-full.html">{string("Current: Full Demo (all of the features)")}</a></li>
      <li><a href="indexJs.html">{string("Full Demo JS (JS equilvalent of Full Demo)")}</a></li>
      <li><a href="features.html">{string("See Features (this page can be cached)")}</a></li>
    </ul>
  </div>
}
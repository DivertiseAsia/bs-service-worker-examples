open ReasonReact;
open BsServiceWorker;

[@react.component]
let make = () => {
  let serviceWorkerRegistration = (_) => {
    switch(ServiceWorker.maybeServiceWorker) {
      | None => {
        Js.log("[App] Service worker does *not* support");
      }
      | Some(notificationController) => {
        Js.log("[App] Service worker supports");
        Js.Promise.(ServiceWorker.Container.register(notificationController,"demo-sw.js")
          |> then_((registration) => {
            Js.log2("[App] Registration sw: ", registration);
            ServiceWorker.Container.addEventListener(registration, "updatefound", {
              _ => {
                Js.log2("update found",registration##installing);
              }
            })
            resolve();
          })
          |> catch(e => {
            Js.log2("[App] Registration failed: ", e);
            resolve()
          })
        )|> ignore;
      }
    };
  };
  <div>
    <h1>{string("Registration Demo")}</h1>
    <h2>{string("Your Browser:")}</h2>
    <button onClick=serviceWorkerRegistration>{string("Registration")}</button>
    <h2>{string("Other Demos")}</h2>
    <ul>
      <li><a href="index.html">{string("Minimal Demo")}</a></li>
      <li><a href="demo-full.html">{string("Full Demo (all of the features)")}</a></li>
      <li><a href="indexJS.html">{string("Full Demo JS (JS equilvalent of Full Demo)")}</a></li>
      <li><a href="demo-notification.html">{string("Notification Demo")}</a></li>
      <li><a href="demo-registration.html">{string("Current:Registration Demo")}</a></li>
      <li><a href="features.html">{string("See Features (this page can be cached)")}</a></li>
    </ul>
  </div>
}
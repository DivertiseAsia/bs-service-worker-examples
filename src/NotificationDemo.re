open ReasonReact;
open BsServiceWorker;

type state = {
  supported: bool,
  permission: option(Notification.Permission.t),
  message: string,
};

type action =
  | ReadPermission(option(Notification.Permission.t))
  | ChangeMessage(string);

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
        | ReadPermission(permission) => {...state, permission}
        | ChangeMessage(message) => {...state, message: message}
      },
      {
        supported: Notification.maybeNotification !== None, 
        permission: switch(Notification.maybeNotification) {
          | None => None
          | Some(x) => Some(x##permission)
        },
        message: "Hello World!"
      }
  );
  let requestPermission = (_) => {
    switch(Notification.maybeNotification) {
      | None => {
        Js.log("[App] Browser does *not* support notifications");
      }
      | Some(notificationController) => {
        Js.log("[App] Browser supports notifications");
        open Notification;
        Js.Promise.(notificationController->requestPermission
          |> then_((p:Permission.t) => {
            Js.log2("[App] Notification permission is: ", p);
            dispatch(ReadPermission(Some(p)))
            resolve();
          })
          |> catch(e => {
            Js.log2("[App] Notification permission failed: ", e);
            dispatch(ReadPermission(None))
            resolve()
          })
        ) |> ignore;
      }
    };
  };
  let sendNotification = (_) => {
    switch(Notification.maybeNotification) {
      | None => {
        Js.log("[App] Browser does *not* support notifications");
      }
      | Some(x) => {
        open Notification;
        Js.log(x->permission);
        createNotification(state.message)
        Js.log("[App] Attempting to notify by saying: " ++ state.message)
      }
    }
  };
  <div>
    <h1>{string("Notification Demo")}</h1>
    <h2>{string("Your Browser:")}</h2>
    <table>
      <tbody>
        <tr key="supported"><td>{string("Supports Notification?")}</td><td>{successIndicator(state.supported, "supported")}</td></tr>
        <tr key="registration"><td>{string("Permission for this site:")}</td><td>{switch(state.permission) {
          | Some(x) when x == Notification.Permission.granted => string("granted")
          | Some(x) when x == Notification.Permission.denied => string("denied")
          | Some(x) when x == Notification.Permission.default => string("default")
          | _ => string("unknown")
        }}</td></tr>
        {switch(state.permission) {
          | Some(x) when x == Notification.Permission.granted =>
          {
            <tr key="permission">
              <td>{string("Send Message:")}</td>
              <td>
                <input type_="text" value=state.message onChange={event => dispatch(ChangeMessage(ReactEvent.Form.target(event)##value))} />
                <button onClick=sendNotification>{string("Send")}</button>
              </td>
            </tr>
          }
          | Some(x) when x == Notification.Permission.denied =>
          {
            <tr key="permission"><td>{string("Permission Denied:")}</td><td>{string("User has to allow manually")}</td></tr>
          }
          | _ => <tr key="permission"><td>{string("Permission:")}</td><td><button onClick=requestPermission>{string("Ask for Permission")}</button></td></tr>
        }}
        // <tr key="unregisterSuccess">
        //   <td>{string("Unregister?")}</td>
        //   <td>{switch(state.registration, state.unregisterSuccess) {
        //     | (Some(_), false) => <button id="unregister" onClick=unregisterServiceWorker>{string("Unregister")}</button>
        //     | _ => successIndicator(state.unregisterSuccess, "unregisterSuccess")
        //   }}
        //   </td>
        // </tr>
      </tbody>
    </table>
    <h2>{string("Other Demos")}</h2>
    <ul>
      <li><a href="index.html">{string("Minimal Demo")}</a></li>
      <li><a href="demo-full.html">{string("Full Demo (all of the features)")}</a></li>
      <li><a href="indexJS.html">{string("Full Demo JS (JS equilvalent of Full Demo)")}</a></li>
      <li><a href="demo-notification.html">{string("Current:Notification Demo")}</a></li>
      <li><a href="features.html">{string("See Features (this page can be cached)")}</a></li>
    </ul>
  </div>
}
var CACHE_NAME = 'my-site-cache-v1';
var urlsToCache = [
  'features.html',
  'images/markingoffline.png',
  'images/crabs.jpg'
];

self.addEventListener('install', function(event) {
  // Perform install steps
  event.waitUntil(
    caches.open(CACHE_NAME)
      .then(function(cache) {
        console.log('[Demo ServiceWorker] Opened cache');
        return cache.addAll(urlsToCache);
      })
  );
});

self.addEventListener('fetch', function(event) {
  event.respondWith(
    caches.match(event.request)
      .then(function(response) {
        // Cache hit - return response
        if (response) {
          console.log("[Demo ServiceWorker] Cache hit");
          return response;
        }
        return fetch(event.request);
      }
    )
  );
});
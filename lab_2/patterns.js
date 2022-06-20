class Magazine {

   constructor() {
      this.state = new ReadyForPushNotificationState(this);
      this.staff = [];
      this.articles = [];
      this.followers = [];

   }
   subscribe(follower) {
      this.followers.push(follower);
   }
   unsubscribe(follower) {
      this.followers.splice(this.followers.find(obj => {
         return obj.name === follower.name
      }), 1);
   }
   updateUser(follower) {
      let result = this.followers.find(obj => {
         return obj.name === follower.name
      });
      if (result) {
         result.topics = follower.topics;
      } else {
         this.subscribe(follower);
      }

   }
   notifyAll() {
      for (let follower of this.followers) {
         for (let topic of follower.topics) {
            for (let article of this.articles) {
               if (topic === article.type) {
                  follower.onUpdate(article.text);
               }
            }
         }
      }
   }

}
class State {
   constructor(magazine) {
      this.magazine = magazine;
   }

}
class ReadyForPushNotificationState extends State {
   constructor(magazine) {
      super(magazine);
      this.state = 'ReadyForPushNotificationState';

   }
   approve(name) {

      console.log(`Hello ${name}. You can't approve.We don't have enough publicashions`);
   }
   publish(name) {
      console.log(`Hello ${name}. You can't publish.We are creating publicashions now`);
   }

}
class ReadyForApproveState extends State {
   constructor(magazine) {
      super(magazine);
      this.state = 'ReadyForApproveState';
   }
   approve(name) {
      console.log(`Hello ${name}. You've approved the changes`);
      this.magazine.state = new ReadyForPublishState(this.magazine);
   }
   publish(name) {
      console.log(`Hello ${name}. You can't publish.We don't have a manager's approval`);
   }
}
class ReadyForPublishState extends State {
   constructor(magazine) {
      super(magazine);
      this.state = 'ReadyForPublishState';
   }
   approve(name) {
      console.log(`Hello ${name}.Publications have been already approved by you.`);
   }
   publish(name) {
      console.log(`Hello ${name}.You've recently published publications.`);
      this.magazine.state = new PublishInProgressState(this.magazine);
      this.magazine.notifyAll();

      setTimeout(() => {

         this.magazine.staff = [];
         this.magazine.articles = [];
         this.magazine.followers = [];
         this.magazine.state = new ReadyForPushNotificationState(this.magazine);
      }, 60000);
   }
}
class PublishInProgressState extends State {
   constructor(magazine) {
      super(magazine);
      this.state = 'PublishInProgressState';
   }
   approve(name) {
      console.log(`Hello ${name}. While we are publishing we can't do any actions.`);
   }
   publish(name) {
      console.log(`Hello ${name}. While we are publishing we can't do any actions.`);
   }

}
class MagazineEmployee {
   constructor(name, role, magazine) {
      this.name = name;
      this.role = role;
      this.magazine = magazine;
      this.magazine.staff.push(name);
   }
   addArticle(text) {
      if (this.role === 'manager') {
         console.log("you don't have a permission to do it"); return;
      } else {
         let obj = { 'type': this.role, 'text': text }
         this.magazine.articles.push(obj);
         if (this.magazine.articles.length >= 5) {
            this.magazine.state = new ReadyForApproveState(this.magazine);
         }
      }
   }
   approve() {
      if (this.role !== 'manager') {
         console.log("you don't have a permission to do it"); return
      }
      this.magazine.state.approve(this.name);
   }
   publish() {

      this.magazine.state.publish(this.name);
   }
}

class Follower {
   constructor(name) {
      this.name = name;
      this.topics = [];
   }
   subscribeTo(magazine, topic) {
      if (this.topics.includes(topic)) {
 console.log('You are already subscribed to this topic.'); return; 
}
      this.topics.push(topic);
      magazine.updateUser(this);
   }
   unsubscribeFrom(magazine, topic) {
      let index = this.topics.indexOf(topic);
      if (index <= -1) {
         console.log("Sorry, such topic wasn't found in your subscriptions' list"); return;
      }
      if (this.topics.length > 1) {
         this.topics.splice(index, 1);
         magazine.updateUser(this);
      } else {
         magazine.unsubscribe(this);
      }
   }
   onUpdate(data) {
      console.log(data, this.name);
   }
}




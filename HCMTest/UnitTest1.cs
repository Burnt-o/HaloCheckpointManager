using Microsoft.VisualStudio.TestTools.UnitTesting;
using HCM.Model;
using HCM.Views;
using System;
using System.Collections.Generic;
namespace HCMTest
{
    [TestClass]
    public class UnitTest1
    {

        [TestMethod]
        public void TestMethod1()
        {
            HCMWindow window = new HCMWindow();
            HCMWindowView view = new HCMWindowView();
            List<GameSaveFolder> folders = new List<GameSaveFolder>();
            GameSaveCollection testCollection1 = new GameSaveCollection();
            testCollection1.Add(new GameSave("My First Checkpoint", DateTime.Now, "a10", 3, TimeSpan.FromSeconds(6), DateTime.Now));

            GameSaveCollection testCollection2 = new GameSaveCollection();

            GameSaveFolder folder1 = new GameSaveFolder("folder1", testCollection1);
            GameSaveFolder folder2 = new GameSaveFolder("folder2", testCollection2);

            folders.Add(folder1);
            folders.Add(folder2);
            view.ChangeTreeViewItemSource(folders);


            var item = folders[0].GameSaveCollection.GameSaves[0];

            Assert.AreEqual("a10", item.LevelName);

        }
    }
}